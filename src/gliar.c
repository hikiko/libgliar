/*
libgliar - a library that can fake the OpenGL context info returned by
the glGet OpenGL calls

Copyright (C) 2013 Canonical Ltd

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.	If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pwd.h>
#include <GL/gl.h>
#include "cfg.h"

#ifndef GL_NUM_SHADING_LANGUAGE_VERSIONS
#define GL_NUM_SHADING_LANGUAGE_VERSIONS	0x82E9
#endif

static int init_valid_extensions(void);

static int done_init;

static const GLubyte* (*gl_get_string)(GLenum);
static const GLubyte* (*gl_get_stringi)(GLenum, GLuint);
static const void* (*gl_get_integerv)(GLenum, GLint*);

/*static const void* (*gl_get_booleanv)(GLenum, GLboolean*);
static const void* (*gl_get_doublev)(GLenum, GLdouble*);
static const void* (*gl_get_floatv)(GLenum, GLfloat*);
static const void* (*gl_get_integer64v)(GLenum, GLint64*);

static const void* (*gl_get_booleani_v)(GLenum, GLuint, GLboolean*);
static const void* (*gl_get_doublei_v)(GLenum, GLuint, GLdouble*);
static const void* (*gl_get_floati_v)(GLenum, GLuint, GLfloat*);
static const void* (*gl_get_integeri_v)(GLenum, GLuint, GLint*);
static const void* (*gl_get_integer64i_v)(GLenum, GLuint, GLint64*);*/

static struct cfgopt *cfglist;


static int init(void)
{
	if(done_init) {
		return 0;
	}

	gl_get_string = dlsym(RTLD_NEXT, "glGetString");
	gl_get_stringi = dlsym(RTLD_NEXT, "glGetStringi");
	gl_get_integerv = dlsym(RTLD_NEXT, "glGetIntegerv");

	/*gl_get_booleanv = dlsym(RTLD_NEXT, "glGetBooleanv");
	gl_get_doublev = dlsym(RTLD_NEXT, "glGetDoublev");
	gl_get_floatv = dlsym(RTLD_NEXT, "glGetFloatv");
	gl_get_integer64v = dlsym(RTLD_NEXT, "glGetInteger64v");

	gl_get_booleani_v = dlsym(RTLD_NEXT, "glGetBooleani_v");
	gl_get_doublei_v = dlsym(RTLD_NEXT, "glGetDoublei_v");
	gl_get_floati_v = dlsym(RTLD_NEXT, "glGetFloati_v");
	gl_get_integeri_v = dlsym(RTLD_NEXT, "glGetIntegeri_v");
	gl_get_integer64i_v = dlsym(RTLD_NEXT, "glGetInteger64i_v");*/

	if(init_valid_extensions() == -1) {
		fprintf(stderr, "GLIAR: failed to initialize the valid extension list, might end up with unavailable extensions!\n");
	}

	if(!(cfglist = gliar_load_cfg("gliar.conf"))) {
		struct passwd *pw;
		char *homedir, *path;

		if((pw = getpwuid(getuid()))) {
			homedir = pw->pw_dir;
		} else {
			homedir = getenv("HOME");
		}

		if(homedir) {
			path = alloca(strlen(homedir) + strlen(".gliar.conf") + 2);
			sprintf(path, "%s/.gliar.conf", homedir);

			cfglist = gliar_load_cfg(path);
		}
	}

	done_init = 1;
	return 0;
}

static int init_valid_extensions(void)
{
	int i, num_ext, prev_space = 0;
	const char *gl_ext_str;
	char *ext_str, *tok, *ptr, **ext_table;

	/* initialize the list of valid extensions */
	if(!(gl_ext_str = (const char*)gl_get_string(GL_EXTENSIONS))) {
		return -1;
	}

	if(!(ext_str = malloc(strlen(gl_ext_str) + 1))) {
		return -1;
	}
	strcpy(ext_str, gl_ext_str);

	/* count the extensions */
	num_ext = 0;
	ptr = ext_str;
	while(*ptr) {
		if(isspace(*ptr) && prev_space == 0) {
			prev_space = 1;
			num_ext++;
		} else {
			prev_space = 0;
		}
		ptr++;
	}

	/* allocate extension table */
	if(!(ext_table = malloc(num_ext * sizeof *ext_table))) {
		free(ext_str);
		return -1;
	}

	/* setup the ext_table slots to point to the start of each substring (extension) */
	for(i=0; i<num_ext; i++) {
		if(!(tok = strtok(i == 0 ? ext_str : 0, " \t\v\n\r"))) {
			fprintf(stderr, "DEBUG: strtok returned 0 at token %d\n", i);
			num_ext = i;
		}
		ext_table[i] = tok;
	}

	gliar_value_set("extensions", ext_table, num_ext);

	free(ext_table);
	free(ext_str);
	return 0;
}

const GLubyte *glGetString(GLenum name)
{
	const char *key;
	const struct cfgopt *option;

	init();

	switch(name) {
	case GL_VENDOR:
		key = "vendor";
		break;

	case GL_VERSION:
		key = "version";
		break;

	case GL_EXTENSIONS:
		key = "extensions";
		break;

	case GL_RENDERER:
		key = "renderer";
		break;

	case GL_SHADING_LANGUAGE_VERSION:
		key = "sl version";
		break;

	default:
		key = 0;
	}

	if(key && (option = gliar_find_opt(cfglist, key))) {
		return (const GLubyte*)option->conc_vals;
	}

	return gl_get_string(name);
}

const GLubyte *glGetStringi(GLenum name, GLuint index)
{
	char *key;
	const struct cfgopt *option;

	init();

	switch(name) {
	case GL_EXTENSIONS:
		key = "extensions";
		break;

	case GL_SHADING_LANGUAGE_VERSION:
		key = "sl version";
		break;

	default:
		key = 0;
	}

	if(key && (option = gliar_find_opt(cfglist, key))) {
		return (const GLubyte*)option->str_val[index];
	}

	return gl_get_stringi(name, index);
}

void glGetIntegerv(GLenum name, GLint *val)
{
	char *key;
	const struct cfgopt *option;

	init();

	switch(name) {
	case GL_NUM_EXTENSIONS:
		if(1) {
			key = "extensions";
		} else {
	case GL_NUM_SHADING_LANGUAGE_VERSIONS:
			key = "sl version";
		}
		if(key && (option = gliar_find_opt(cfglist, key))) {
			*val = option->str_count;
			return;
		}
		break;

	case GL_MAX_TEXTURE_UNITS:
		key = "max texture units";
		break;

	case GL_MAX_TEXTURE_SIZE:
		key = "max texture size";
		break;

	case GL_MAX_TEXTURE_COORDS:
		key = "max texture coordinates";
		break;

	default:
		key = 0;
	}

	if(key && (option = gliar_find_opt(cfglist, key)) && option->type == GLIAR_NUMBER) {
		*val = option->num_val;
		return;
	}

	gl_get_integerv(name, val);
}

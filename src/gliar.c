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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pwd.h>
#include <GL/gl.h>
#include "cfg.h"

static int done_init;
static const GLubyte* (*gl_get_string)(GLenum);
static struct cfgopt *cfglist;

static int init(void)
{
	if(done_init) {
		return 0;
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

	gl_get_string = dlsym(RTLD_NEXT, "glGetString");

	done_init = 1;
	return 0;
}

const GLubyte *glGetString(GLenum name)
{
	const char *key, *value;

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

	if(key && (value = gliar_find_opt(cfglist, key))) {
		return value;
	}

	return gl_get_string(name);
}

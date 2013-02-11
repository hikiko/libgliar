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

	default:
		key = 0;
	}

	if(key && (value = gliar_find_opt(cfglist, key))) {
		return value;
	}

	return gl_get_string(name);
}

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
#define GL_GLEXT_PROTOTYPES	1
#include <GL/glut.h>
#include "glext.h"

#if !defined(GL_VERSION_3_0) || !GL_VERSION_3_0
#error "OpenGL version 3.0 required"
#endif

int main(int argc, char **argv)
{
	int i, num_ext;

	glutInit(&argc, argv);
	glutCreateWindow("ext-by-index");

	glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
	printf("Extensions:\n");
	for(i=0; i<num_ext; i++) {
		printf("%d: %s\n", i, (char*)glGetStringi(GL_EXTENSIONS, i));
	}
	return 0;
}

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
#include <GL/glut.h>

#define GET_INTEGER(x)	(glGetIntegerv(x, &val), val)

int main(int argc, char **argv)
{
	int val;

	glutInit(&argc, argv);
	glutCreateWindow("glinfo");

	printf("vendor: %s\n", glGetString(GL_VENDOR));
	printf("renderer: %s\n", glGetString(GL_RENDERER));
	printf("version: %s\n", glGetString(GL_VERSION));
	printf("shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("major version: %d\n", GET_INTEGER(GL_MAJOR_VERSION));
	printf("minor version: %d\n", GET_INTEGER(GL_MINOR_VERSION));
	printf("max texture units: %d\n", GET_INTEGER(GL_MAX_TEXTURE_UNITS));
	printf("max texture image units: %d\n", GET_INTEGER(GL_MAX_TEXTURE_IMAGE_UNITS));
	printf("max combined texture image units: %d\n", GET_INTEGER(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS));
	printf("max vertex texture image units: %d\n", GET_INTEGER(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS));
	printf("max texture size: %d\n", GET_INTEGER(GL_MAX_TEXTURE_SIZE));
	printf("max cube map texture size: %d\n", GET_INTEGER(GL_MAX_CUBE_MAP_TEXTURE_SIZE));
	printf("max texture coordinates: %d\n", GET_INTEGER(GL_MAX_TEXTURE_COORDS));
	printf("max vertex attributes: %d\n", GET_INTEGER(GL_MAX_VERTEX_ATTRIBS));
	printf("max vertex uniform vectors: %d\n", GET_INTEGER(GL_MAX_VERTEX_UNIFORM_VECTORS));
	printf("max fragment uniform vectors: %d\n", GET_INTEGER(GL_MAX_FRAGMENT_UNIFORM_VECTORS));
	printf("max varying vectors: %d\n", GET_INTEGER(GL_MAX_VARYING_VECTORS));
	printf("max color attachments: %d\n", GET_INTEGER(GL_MAX_COLOR_ATTACHMENTS_EXT));
	printf("max renderbuffer size ext: %d\n", GET_INTEGER(GL_MAX_RENDERBUFFER_SIZE_EXT));
	return 0;
}

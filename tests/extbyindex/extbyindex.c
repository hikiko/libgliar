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

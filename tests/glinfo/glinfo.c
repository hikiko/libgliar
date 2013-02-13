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

	printf("texture units: %d\n", GET_INTEGER(GL_MAX_TEXTURE_UNITS));
	printf("max texture size: %d\n", GET_INTEGER(GL_MAX_TEXTURE_SIZE));
	return 0;
}

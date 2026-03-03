#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/glew.h>
#include <GL/glut.h>

GLuint loadTexture(const char* filename);
GLuint loadRepeatingTexture(const char* filename);
#endif
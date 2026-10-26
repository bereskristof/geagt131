#ifndef LOAD_TEX_H
#define LOAD_TEX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <stdio.h>

typedef GLubyte pixel[4];

GLuint load_texture(char* filename);

#endif // LOAD_TEX_H

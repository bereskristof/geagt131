#include "load_tex.h"

GLuint load_texture(char* filename) {
	SDL_Surface* surface;
	GLuint texture_name;

	surface = IMG_Load(filename);

	glGenTextures(1, &texture_name);

	glBindTexture(GL_TEXTURE_2D, texture_name);

	int bytes_per_pixel = surface->format->BytesPerPixel;
	GLenum format;
	if (bytes_per_pixel == 3) format = GL_RGB;
	if (bytes_per_pixel == 4) format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, (pixel*)(surface->pixels));

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture_name;
}

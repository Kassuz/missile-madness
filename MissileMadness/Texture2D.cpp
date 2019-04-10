#include "Texture2D.h"

#include <glad/glad.h>


Texture2D::Texture2D() : width(0), height(0)
{
	glGenTextures(1, &this->ID);
}


Texture2D::~Texture2D()
{
	glDeleteTextures(1, &this->ID);
}

void Texture2D::Generate(UInt32 width, UInt32 height, UInt8* data)
{
	this->width  = width;
	this->height = height;

	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Use()
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}

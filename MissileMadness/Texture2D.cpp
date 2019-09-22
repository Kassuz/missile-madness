//--------------------------------------------------------------------------------------------
// Modified from examples at LearnOpenGL (learnopengl.com) by Joy de Vries https://twitter.com/JoeyDeVriez
// https://learnopengl.com/code_viewer.php?code=in-practice/breakout/texture
// Licenced under CC BY-NC 4.0 https://creativecommons.org/licenses/by-nc/4.0/
//--------------------------------------------------------------------------------------------

#include "Texture2D.h"

#include <glad/glad.h>


Texture2D::Texture2D() : m_Width(0), m_Height(0)
{
	glGenTextures(1, &this->m_ID);
}


Texture2D::~Texture2D()
{
	glDeleteTextures(1, &this->m_ID);
}

void Texture2D::Generate(UInt32 width, UInt32 height, UInt8* data)
{
	this->m_Width  = width;
	this->m_Height = height;

	glBindTexture(GL_TEXTURE_2D, this->m_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Use()
{
	glBindTexture(GL_TEXTURE_2D, this->m_ID);
}

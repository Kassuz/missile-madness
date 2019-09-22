//--------------------------------------------------------------------------------------------
// Modified from examples at LearnOpenGL (learnopengl.com) by Joy de Vries https://twitter.com/JoeyDeVriez
// https://learnopengl.com/code_viewer.php?code=in-practice/breakout/texture.h
// Licenced under CC BY-NC 4.0 https://creativecommons.org/licenses/by-nc/4.0/
//--------------------------------------------------------------------------------------------

#pragma once

#include "Types.h"

class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	UInt32 m_ID;
	UInt32 m_Width, m_Height;

	void Generate(UInt32 width, UInt32 height, UInt8* data);
	void Use();
};


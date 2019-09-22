//--------------------------------------------------------------------------------------------
// Modified from examples at LearnOpenGL (learnopengl.com) by Joy de Vries https://twitter.com/JoeyDeVriez
// https://learnopengl.com/code_viewer.php?code=in-practice/breakout/text_renderer
// Licenced under CC BY-NC 4.0 https://creativecommons.org/licenses/by-nc/4.0/
//--------------------------------------------------------------------------------------------

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Engine.h"

void TextRenderer::Init(UInt32 width, UInt32 height)
{
	m_ScreenWidth  = width;
	m_ScreenHeight = height;

	// Load and configure shader
	this->m_TextShader = ResourceManager::Instance().LoadShader("Text");
	this->m_TextShader->Use();
	this->m_TextShader->SetMat4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f));
	this->m_TextShader->SetInt("text", 0);
	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &this->m_VAO);
	glGenBuffers(1, &this->m_VBO);
	glBindVertexArray(this->m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TextRenderer& TextRenderer::Instance()
{
	static TextRenderer instance;
	return instance;
}

void TextRenderer::Load(std::string font, UInt32 fontSize)
{
	// First clear the previously loaded Characters
	this->m_Characters.clear();
	// Then initialize and load the FreeType library
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) // All functions return a value different than 0 whenever an error occurred
		Debug::LogError("Could not init FreeType Library");
	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face))
		Debug::LogError("Failed to load font");
	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Then for the first 128 ASCII characters, pre-load/compile their characters and store them
	for (UInt8 c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			Debug::LogError("Failed to load Glyph for char: " + c);
			continue;
		}
		// Generate texture
		UInt32 texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		m_Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, float activeTime, Color color)
{
	RenderTextObj* textToRender = new RenderTextObj();
	textToRender->m_Text = text;
	textToRender->m_X = x;
	textToRender->m_Y = y;
	textToRender->m_Scale = scale;
	textToRender->m_ActiveTime = activeTime;
	textToRender->m_TextColor = color;

	textToRender->m_Next = m_RenderHead;
	if (m_RenderHead != nullptr) m_RenderHead->m_Previous = textToRender;
	m_RenderHead = textToRender;
}

void TextRenderer::RenderTextWorldSpace(std::string text, float x, float y, float scale, float activeTime, Color color)
{
	float newX = x + (m_ScreenWidth  / 2.0f);
	float newY = y - (m_ScreenHeight / 2.0f);
	if (newY < 0.0f) newY *= -1.0f;
	RenderText(text, newX, newY, scale, activeTime, color);
}

void TextRenderer::RenderAllText()
{
	this->m_TextShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->m_VAO);

	RenderTextObj* cur = m_RenderHead;
	while (cur != nullptr)
	{
		this->m_TextShader->SetVec3("textColor", cur->m_TextColor);

		float x = cur->m_X;
		float y = cur->m_Y;
		float scale = cur->m_Scale;

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = cur->m_Text.begin(); c != cur->m_Text.end(); c++)
		{
			Character ch = m_Characters[*c];

			float xpos = x + ch.m_Bearing.x * scale;
			float ypos = y + (this->m_Characters['H'].m_Bearing.y - ch.m_Bearing.y) * scale;

			float w = ch.m_Size.x * scale;
			float h = ch.m_Size.y * scale;
			// Update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 0.0 },
				{ xpos,     ypos,       0.0, 0.0 },

				{ xpos,     ypos + h,   0.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 1.0 },
				{ xpos + w, ypos,       1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.m_TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph
			x += (ch.m_Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
		}

		cur->m_ActiveTimer += Time::deltaTime;
		if (cur->m_ActiveTimer >= cur->m_ActiveTime)
		{
			if (cur->m_Previous != nullptr) cur->m_Previous->m_Next = cur->m_Next;
			if (cur->m_Next != nullptr) cur->m_Next->m_Previous = cur->m_Previous;
			if (m_RenderHead == cur) m_RenderHead = cur->m_Next;
			RenderTextObj* temp = cur;
			cur = cur->m_Next;
			delete temp;
		}
		else
		{
			cur = cur->m_Next;
		}
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
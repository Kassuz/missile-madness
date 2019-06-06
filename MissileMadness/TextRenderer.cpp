#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Engine.h"

void TextRenderer::Init(UInt32 width, UInt32 height)
{
	// Load and configure shader
	this->textShader = ResourceManager::Instance().LoadShader("Text");
	this->textShader->Use();
	this->textShader->SetMat4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f));
	this->textShader->SetInt("text", 0);
	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TextRenderer& TextRenderer::Instance()
{
	// TODO: insert return statement here
	static TextRenderer instance;
	return instance;
}

void TextRenderer::Load(std::string font, UInt32 fontSize)
{
	// First clear the previously loaded Characters
	this->Characters.clear();
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
		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, float activeTime, Color color)
{
	RenderTextObj* textToRender = new RenderTextObj();
	textToRender->text = text;
	textToRender->x = x;
	textToRender->y = y;
	textToRender->scale = scale;
	textToRender->activeTime = activeTime;
	textToRender->textColor = color;

	textToRender->next = renderHead;
	if (renderHead != nullptr) renderHead->previous = textToRender;
	renderHead = textToRender;
}

void TextRenderer::RenderAllText()
{
	this->textShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

	RenderTextObj* cur = renderHead;
	while (cur != nullptr)
	{
		this->textShader->SetVec3("textColor", cur->textColor);

		float x = cur->x;
		float y = cur->y;
		float scale = cur->scale;

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = cur->text.begin(); c != cur->text.end(); c++)
		{
			Character ch = Characters[*c];

			float xpos = x + ch.Bearing.x * scale;
			float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;
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
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph
			x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
		}

		cur->activeTimer += Time::deltaTime;
		if (cur->activeTimer >= cur->activeTime)
		{
			if (cur->previous != nullptr) cur->previous->next = cur->next;
			if (cur->next != nullptr) cur->next->previous = cur->previous;
			if (renderHead == cur) renderHead = cur->next;
			RenderTextObj* temp = cur;
			cur = cur->next;
			delete temp;
		}
		else
		{
			cur = cur->next;
		}
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
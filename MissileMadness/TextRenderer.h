#pragma once

#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Types.h"
#include "Color.h"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character 
{
	UInt32 TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	UInt32 Advance;     // Horizontal offset to advance to next glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer
{
public:
	static TextRenderer& Instance();

	// Holds a list of pre-compiled Characters
	std::map<char, Character> Characters;
	
	void Init(UInt32 width, UInt32 height);
	// Pre-compiles a list of characters from the given font
	void Load(std::string font, UInt32 fontSize);
	// Renders a string of text using the precompiled list of characters
	void RenderText(std::string text, float x, float y, float scale, float activeTime = 0.0f, Color color = Color::Black());
	void RenderTextWorldSpace(std::string text, float x, float y, float scale, float activeTime = 0.0f, Color color = Color::Black());
	void RenderAllText();

private:
	// Render state
	GLuint VAO, VBO;
	Shader* textShader;

	TextRenderer() {};
	TextRenderer(const TextRenderer& other) = delete;
	TextRenderer& operator=(const TextRenderer& other) = delete;

	struct RenderTextObj
	{
		std::string text;
		Color textColor;
		float x, y, scale;
		float activeTime;
		float activeTimer = 0.0f;

		RenderTextObj* next = nullptr, * previous = nullptr;
	};

	RenderTextObj* renderHead;

	UInt32 screenHeight = 0U, screenWidth = 0U;
};

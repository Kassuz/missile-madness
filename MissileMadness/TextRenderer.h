//--------------------------------------------------------------------------------------------
// Modified from examples at LearnOpenGL (learnopengl.com) by Joy de Vries https://twitter.com/JoeyDeVriez
// https://learnopengl.com/code_viewer.php?code=in-practice/breakout/text_renderer.h
// Licenced under CC BY-NC 4.0 https://creativecommons.org/licenses/by-nc/4.0/
//--------------------------------------------------------------------------------------------

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
	UInt32 m_TextureID;   // ID handle of the glyph texture
	glm::ivec2 m_Size;    // Size of glyph
	glm::ivec2 m_Bearing; // Offset from baseline to left/top of glyph
	UInt32 m_Advance;     // Horizontal offset to advance to next glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer
{
public:
	static TextRenderer& Instance();

	// Holds a list of pre-compiled Characters
	std::map<char, Character> m_Characters;
	
	void Init(UInt32 width, UInt32 height);
	// Pre-compiles a list of characters from the given font
	void Load(std::string font, UInt32 fontSize);
	// Renders a string of text using the precompiled list of characters
	void RenderText(std::string text, float x, float y, float scale, float activeTime = 0.0f, Color color = Color::Black());
	void RenderTextWorldSpace(std::string text, float x, float y, float scale, float activeTime = 0.0f, Color color = Color::Black());
	void RenderAllText();

private:
	// Render state
	UInt32 m_VAO, m_VBO;
	Shader* m_TextShader;

	TextRenderer() {};
	TextRenderer(const TextRenderer& other) = delete;
	TextRenderer& operator=(const TextRenderer& other) = delete;

	struct RenderTextObj
	{
		std::string m_Text;
		Color m_TextColor;
		float m_X, m_Y, m_Scale;
		float m_ActiveTime;
		float m_ActiveTimer = 0.0f;

		RenderTextObj* m_Next = nullptr, * m_Previous = nullptr;
	};

	RenderTextObj* m_RenderHead;

	UInt32 m_ScreenHeight = 0U, m_ScreenWidth = 0U;
};

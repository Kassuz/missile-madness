#pragma once

#include "Types.h"
#include "Window.h"

#include <string>

class RenderingEngine
{
public:
	// Initializes renderers
	static void Init(WindowPtr window, const std::string& font);

	// Do all rendering steps
	static void Render();

private:
	static WindowPtr s_Window; // Could be also only one static window somewhere?
};


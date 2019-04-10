#include "Debug.h"

#include <Windows.h>
#include <iostream>

#include <glad/glad.h>

#include "Color.h"
#include "Shader.h"
#include "Types.h"

HANDLE Debug::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//Shader Debug::debugShader = Shader("../basic.vert", "../basic.frag");

void Debug::Log(const std::string& text)
{
#ifdef _DEBUG
	std::cout << "LOG   >> " << text << std::endl;
#endif // _DEBUG
}

void Debug::LogError(const std::string& text)
{
#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, 12);
	std::cout << "ERROR >> ";
	SetConsoleTextAttribute(hConsole, 7);
	std::cout << text << std::endl;
#endif // _DEBUG
}


void Debug::LogWarning(const std::string& text)
{
#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, 14);
	std::cout << "WARN  >> ";
	SetConsoleTextAttribute(hConsole, 7);
	std::cout << text << std::endl;
#endif // _DEBUG
}

void Debug::DrawLine(glm::vec3 start, glm::vec3 end, Color color)
{
	//UInt32 VAO, VBO;
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);

	//float vertices[] = { start.x, start.y, start.z, end.x, end.y, end.z };

	//glBindVertexArray(VAO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);


	//glDrawArrays(GL_LINES, 0, 6);

}

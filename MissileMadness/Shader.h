//--------------------------------------------------------------------------------------------
// Modified from examples at LearnOpenGL (learnopengl.com) by Joy de Vries https://twitter.com/JoeyDeVriez
// Licenced under CC BY-NC 4.0 https://creativecommons.org/licenses/by-nc/4.0/
//--------------------------------------------------------------------------------------------

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Debug.h"

class Shader
{
public:
	// Program ID
	UInt32 m_ID;

	Shader();
	~Shader();

	// Use Shader
	void Compile(const char* vertexPath, const char* fragmentPath);
	void Use();

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, Int32 value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec2(const std::string& name, float x, float y) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetVec3(const std::string& name, float x, float y, float z) const;
	void SetVec4(const std::string& name, const glm::vec4& value) const;
	void SetVec4(const std::string& name, float x, float y, float z, float w);
	void SetMat2(const std::string& name, const glm::mat2& mat) const;
	void SetMat3(const std::string& name, const glm::mat3& mat) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;

};

inline Shader::Shader() : m_ID(0)
{

}

inline Shader::~Shader()
{
	glDeleteProgram(m_ID);
}

inline void Shader::Compile(const char* vShaderCode, const char* fShaderCode)
{
	UInt32 vertex, fragment;
	Int32 success;
	char infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::string errorText = "SHADER::VERTEX::COMPILATION_FAILED\n";
		errorText += infoLog;
		Debug::LogError(errorText);
	};

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::string errorText = "SHADER::FRAGMENT::COMPILATION_FAILED\n";
		errorText += infoLog;
		Debug::LogError(errorText);
	};

	// Create shader program

	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, fragment);
	glLinkProgram(m_ID);
	// print linking errors if any
	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
		std::string errorText = "SHADER::PROGRAM::LINKING_FAILED\n";
		errorText += infoLog;
		Debug::LogError(errorText);
	}

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

inline void Shader::Use()
{
	glUseProgram(m_ID);
}

inline void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int) value);
}

inline void Shader::SetInt(const std::string& name, Int32 value) const
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

inline void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

inline void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}

inline void Shader::SetVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(m_ID, name.c_str()), x, y);
}

inline void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}

inline void Shader::SetVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(m_ID, name.c_str()), x, y, z);
}

inline void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}

inline void Shader::SetVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(m_ID, name.c_str()), x, y, z, w);
}

inline void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

inline void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

inline void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
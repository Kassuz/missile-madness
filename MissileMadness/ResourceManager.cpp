#include "ResourceManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "stb_image.h"

#include "Shader.h"
#include "Texture2D.h"

#include "Debug.h"


ResourceManager& ResourceManager::Instance()
{
	static ResourceManager instance;
	return instance;
}

Shader* ResourceManager::LoadShader(const std::string& name)
{
	auto shader = shaders.find(name);
	if (shader != shaders.end())
		return shader->second;

	Shader* s = LoadShaderFromFile("../" + name + ".vert", "../" + name + ".frag");
	shaders.emplace(name, s);
	return s;
}

Texture2D* ResourceManager::LoadTexture2D(const std::string& path)
{
	auto tex = textures.find(path);
	if (tex != textures.end())
		return tex->second;

	Texture2D* t = LoadTexture2DFromFile(path);
	textures.emplace(path, t);
	return t;
}

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
	for (auto it : shaders)
	{
		delete it.second;
	}

	for (auto it : textures)
	{
		delete it.second;
	}
}


Shader* ResourceManager::LoadShaderFromFile(const std::string& vertex, const std::string& fragment)
{
	Debug::LogWarning("Loading shader...");
	// 1. retrieve shader code from filepath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// open files
		vShaderFile.open(vertex);
		fShaderFile.open(fragment);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		Debug::LogError("SHADER::FILE_NOT_SUCCESFULLY_READ");
		return nullptr;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	Shader* s = new Shader;
	s->Compile(vShaderCode, fShaderCode);
	return s;
}

Texture2D* ResourceManager::LoadTexture2DFromFile(const std::string& path)
{
	Debug::LogWarning("Loading texture...");
	Int32 width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	UInt8* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		Debug::LogError("Failed to load image at " + path);
		return nullptr;
	}

	Texture2D* tex = new Texture2D;
	tex->Generate(width, height, data);
	stbi_image_free(data);
	return tex;
}

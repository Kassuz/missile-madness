#pragma once

#include <map>
#include <string>

class Shader;
class Texture2D;

class ResourceManager
{
public:
	static ResourceManager& Instance();

	Shader* LoadShader(const std::string& name);
	Texture2D* LoadTexture2D(const std::string& path);

private:
	ResourceManager();
	~ResourceManager();

	ResourceManager(const ResourceManager& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;

	std::map<std::string, Shader*> shaders;
	std::map<std::string, Texture2D*> textures;

	Shader* LoadShaderFromFile(const std::string& vertex, const std::string& fragment);
	Texture2D* LoadTexture2DFromFile(const std::string& path);
};


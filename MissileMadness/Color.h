#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Color
{
public:
	Color();
	Color(float r, float g, float b, float a);
	~Color();

	Color(glm::vec3);
	Color(glm::vec4);
	Color(const Color& other);
	Color& operator=(const Color& other);
	operator glm::vec3();
	operator glm::vec4();

	float r, g, b, a;

	static Color Red()     { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
	static Color Green()   { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
	static Color Blue()    { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
	static Color Magenta() { return Color(1.0f, 0.0f, 1.0f, 1.0f); }
	static Color Cyan()    { return Color(0.0f, 1.0f, 1.0f, 1.0f); }
	static Color Yellow()  { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
	static Color White()   { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
	static Color Black()   { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
};

inline Color::Color() : r(0), g(0), b(0), a(0)
{
}

inline Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
{
}

inline Color::Color(glm::vec3 vec)
{
	this->r = vec.x;
	this->g = vec.y;
	this->b = vec.z;
	this->a = 1.0f;
}

inline Color::Color(glm::vec4 vec)
{
	this->r = vec.x;
	this->g = vec.y;
	this->b = vec.z;
	this->a = vec.w;
}

inline Color::Color(const Color& other)
{
	this->r = other.r;
	this->g = other.g;
	this->b = other.b;
	this->a = other.a;
}

inline Color& Color::operator=(const Color& other)
{
	this->r = other.r;
	this->g = other.g;
	this->b = other.b;
	this->a = other.a;

	return *this;
}

inline Color::operator glm::vec3()
{
	return glm::vec3(r, g, b);
}

inline Color::operator glm::vec4()
{
	return glm::vec4(r, g, b, a);
}

inline Color::~Color()
{
}

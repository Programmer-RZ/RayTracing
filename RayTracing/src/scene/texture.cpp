#include "texture.hpp"

Texture::Texture(std::string type) 
	: type(type)
{}
glm::vec3 Texture::GetColorAt(float u, float v, const glm::vec3& p) const { return glm::vec3(0.0f, 0.0f, 0.0f); }


SolidTexture::SolidTexture(glm::vec3 color) 
	: Texture("solid texture"), color(color)
{}

glm::vec3& SolidTexture::GetColor() {
	return this->color;
}

glm::vec3 SolidTexture::GetColorAt(float u, float v, const glm::vec3& p) const {
	return this->color;
}
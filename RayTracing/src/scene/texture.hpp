#pragma once

#include <string>
#include "glm/glm.hpp"

class Texture {
private:
	std::string type;

public:
	Texture(std::string type);

	std::string GetType() const { return this->type; }

	virtual glm::vec3 GetColorAt(float u, float v, const glm::vec3& p) const;
};

class SolidTexture : public Texture {
private:
	glm::vec3 color;

public:
	SolidTexture(glm::vec3 color);
	
	glm::vec3& GetColor();
	glm::vec3 GetColorAt(float u, float v, const glm::vec3& p) const override;
};
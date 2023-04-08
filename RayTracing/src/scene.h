#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Material {
	std::string name = "Material";

	glm::vec3 Albedo{ 1.0f, 0.0f, 1.0f };
	float roughness = 1.0f;
	float metallic = 0.0f;
};

struct Sphere {
	// default params
	glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
	float radius = 0.5f;

	// name
	std::string name = "Sphere";

	int material_index = 0;
};

struct Scene {
	std::string name = "Scene";

	std::vector<Sphere> spheres = {};
	std::vector<Material> materials = {};
};

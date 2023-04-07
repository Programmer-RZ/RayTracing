#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Sphere {
	// default params
	glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
	float radius = 0.5f;

	// color
	glm::vec3 Albedo{ 1.0f, 0.0f, 1.0f };	

	// name
	std::string name = "Sphere";
};

struct Scene {
	std::string name = "Scene";
	std::vector<Sphere> spheres = {};
};

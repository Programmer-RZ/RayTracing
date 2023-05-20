#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Material {
	std::string name = "Material";

	glm::vec3 Albedo{ 1.0f, 0.0f, 1.0f };
	float roughness = 1.0f;
	float metallic = 0.0f;
	int id;
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

	const int num_of_avail_obj = 1;
	char* objects[1] = {"Sphere"};
	char* selected_object = "Sphere";

	glm::vec3 lightDir = glm::vec3(-0.546f, -0.722f, -0.425f);
	glm::vec3 skycolor = glm::vec3(0.0f, 0.0f, 0.0f);
	std::vector<Sphere> spheres = {};
	std::vector<Material> materials = {};

	void createNewSphere(glm::vec3 camera_pos, glm::vec3 camera_dir) {
		Sphere sphere;
		sphere.name = "Sphere " + std::to_string(this->spheres.size());
		sphere.pos = {
			camera_pos.x + camera_dir.x * 2,
			camera_pos.y + camera_dir.y * 2,
			camera_pos.z + camera_dir.z * 2
		};
		sphere.radius = 0.5f;
		sphere.material_index = 0;

		this->spheres.push_back(sphere);
	}
};

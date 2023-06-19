#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Material {
	std::string name = "Material";

	std::string lighting = "reflect";

	glm::vec3 Albedo{ 1.0f, 0.0f, 1.0f };
	float roughness = 0.0f;
	float EmissionPower = 0.0f;

	int id = 0;

	glm::vec3 GetEmission() const { return this->Albedo * this->EmissionPower; }
};

struct Entity {
};

struct Sphere : public Entity {
	// default params
	glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
	float radius = 0.5f;

	// name
	std::string name = "Sphere";

	int material_index = 0;
};

struct Box : public Entity {
	
	glm::vec3 corner{ 0.0f, 0.0f, 0.0f };
	float width = 0.5f;
	float length = 0.5f;
	float height = 0.5f;

	std::string name = "Box";

	int material_index = 0;
};

struct Scene {
	std::string name = "Scene";

	const int num_of_avail_obj = 1;
	char* objects[1] = {"Sphere"};
	char* selected_object = "Sphere";

	glm::vec3 skycolor = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<Sphere> spheres = {};
	std::vector<Box> boxes = {};

	std::vector<Material> materials = {};

	void createNewSphere(glm::vec3 camera_pos, glm::vec3 camera_dir);
};

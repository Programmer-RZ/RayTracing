#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Material {
	std::string name = "Material";

	std::string lighting = "reflect";

	glm::vec3 Albedo{ 1.0f, 0.0f, 1.0f };
	float roughness = 0.0f;
	float EmissionPower = 1.0f;

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

struct xy_rect : public Entity {
	xy_rect(float x0, float x1, float y0, float y1, float k, int material_index)
		: x0(x0), x1(x1), y0(y0), y1(y1), k(k), material_index(material_index)
	{}
	float x0, x1, y0, y1, k;
	int material_index;
};
struct xz_rect : public Entity {
	xz_rect(float x0, float x1, float z0, float z1, float k, int material_index)
		: x0(x0), x1(x1), z0(z0), z1(z1), k(k), material_index(material_index)
	{}
	float x0, x1, z0, z1, k;
	int material_index;
};
struct yz_rect : public Entity {
	yz_rect(float y0, float y1, float z0, float z1, float k, int material_index)
		: y0(y0), y1(y1), z0(z0), z1(z1), k(k), material_index(material_index)
	{}
	float y0, y1, z0, z1, k;
	int material_index;
};

struct Box : public Entity {
	std::string name = "Box";
	int material_index = 0;
	
	glm::vec3 box_min{ 1.0f, 1.0f, 1.0f };
	glm::vec3 box_max{ 2.0f, 2.0f, 2.0f };

	xy_rect side1 = xy_rect(box_min.x, box_max.x, box_min.y, box_max.y, box_max.z, material_index);
	xy_rect side2 = xy_rect(box_min.x, box_max.x, box_min.y, box_max.y, box_min.z, material_index);

	xz_rect side3 = xz_rect(box_min.x, box_max.x, box_min.z, box_max.z, box_max.y, material_index);
	xz_rect side4 = xz_rect(box_min.x, box_max.x, box_min.z, box_max.z, box_min.y, material_index);

	yz_rect side5 = yz_rect(box_min.y, box_max.y, box_min.z, box_max.z, box_max.x, material_index);
	yz_rect side6 = yz_rect(box_min.y, box_max.y, box_min.z, box_max.z, box_min.x, material_index);
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

	std::string createNewSphere(glm::vec3 camera_pos, glm::vec3 camera_dir);
};

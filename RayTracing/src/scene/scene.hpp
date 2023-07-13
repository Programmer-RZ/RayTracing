#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Material {
	Material(glm::vec3 Albedo, float roughness, float EmissionPower, int id, std::string lighting, std::string name) 
		: Albedo(Albedo), roughness(roughness), EmissionPower(EmissionPower), id(id), lighting(lighting), name(name)
	{}
	std::string name;

	std::string lighting;

	glm::vec3 Albedo;
	
	// metal material
	float roughness;
	
	// diffuse light
	float EmissionPower;

	int id;

	glm::vec3 GetEmission() const { return this->Albedo * this->EmissionPower; }
};


struct Sphere {
	Sphere(glm::vec3 pos, float radius, std::string name, int material_index) 
		: pos(pos), radius(radius), name(name), material_index(material_index)
	{}

	glm::vec3 pos;
	float radius;

	std::string name;

	int material_index;
};

struct xy_rect {
	xy_rect(float x0, float x1, float y0, float y1, float k, int material_index)
		: x0(x0), x1(x1), y0(y0), y1(y1), k(k), material_index(material_index)
	{}
	float x0, x1, y0, y1, k;
	int material_index;
};
struct xz_rect {
	xz_rect(float x0, float x1, float z0, float z1, float k, int material_index)
		: x0(x0), x1(x1), z0(z0), z1(z1), k(k), material_index(material_index)
	{}
	float x0, x1, z0, z1, k;
	int material_index;
};
struct yz_rect {
	yz_rect(float y0, float y1, float z0, float z1, float k, int material_index)
		: y0(y0), y1(y1), z0(z0), z1(z1), k(k), material_index(material_index)
	{}
	float y0, y1, z0, z1, k;
	int material_index;
};

struct Box {
	Box(glm::vec3 box_min, glm::vec3 box_max, std::string name, int material_index) 
		: box_min(box_min), box_max(box_max), name(name), material_index(material_index),

		side1(box_min.x, box_max.x, box_min.y, box_max.y, box_max.z, material_index),
		side2(box_min.x, box_max.x, box_min.y, box_max.y, box_min.z, material_index),
		side3(box_min.x, box_max.x, box_min.z, box_max.z, box_max.y, material_index),
		side4(box_min.x, box_max.x, box_min.z, box_max.z, box_min.y, material_index),
		side5(box_min.y, box_max.y, box_min.z, box_max.z, box_max.x, material_index),
		side6(box_min.y, box_max.y, box_min.z, box_max.z, box_min.x, material_index),
		
		length(box_max.x-box_min.x),
		width(box_max.z-box_min.z),
		height(box_max.y-box_min.y)
	{}

	std::string name;
	int material_index;
	
	glm::vec3 box_min;
	glm::vec3 box_max;
	float length, width, height;

	xy_rect side1;
	xy_rect side2;

	xz_rect side3;
	xz_rect side4;

	yz_rect side5;
	yz_rect side6;

	void updateSides();
};


struct Scene {
	std::string name = "Scene";

	const int num_of_avail_obj = 2;
	char* objects[2] = { "Sphere", "Box" };
	char* selected_object = objects[0];

	glm::vec3 skycolor = glm::vec3(0.5f, 0.8f, 0.9f);

	std::vector<Sphere> spheres = {};
	std::vector<Box> boxes = {};

	std::vector<Material> materials = {};

	std::string createNewSphere(const glm::vec3 camera_pos, const glm::vec3 camera_dir);
	std::string createNewBox(const glm::vec3 camera_pos, const glm::vec3 camera_dir);
};

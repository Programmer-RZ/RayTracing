#include "scene.hpp"

std::string Scene::createNewSphere(glm::vec3 camera_pos, glm::vec3 camera_dir) {
	glm::vec3 pos = {
		camera_pos.x + camera_dir.x * 2,
		camera_pos.y + camera_dir.y * 2,
		camera_pos.z + camera_dir.z * 2
	};

	std::string name = "Sphere " + std::to_string(this->spheres.size());

	float radius = 0.5f;
	int material_index = 0;

	Sphere sphere = Sphere(pos, radius, name, material_index);
	this->spheres.push_back(sphere);

	return name;
}

std::string Scene::createNewBox(glm::vec3 camera_pos, glm::vec3 camera_dir) {
	glm::vec3 box_min = {
		camera_pos.x + camera_dir.x * 2,
		camera_pos.y + camera_dir.y * 2,
		camera_pos.z + camera_dir.z * 2
	};
	glm::vec3 box_max = glm::vec3{ box_min.x+0.5f, box_min.y+0.5f, box_min.z+0.5f };

	std::string name = "Box " + std::to_string(this->boxes.size());

	int material_index = 0;

	Box box = Box(box_min, box_max, name, material_index);
	this->boxes.push_back(box);

	return name;
}

void Box::updateSides() {
	this->box_max = this->box_min + glm::vec3(this->length, this->height, this->width);
	
	glm::vec3 bmin = this->box_min;
	glm::vec3 bmax = this->box_max;

	// side 1
	{
		this->side1.x0 = bmin.x; this->side1.x1 = bmax.x;
		this->side1.y0 = bmin.y, this->side1.y1 = bmax.y;
		this->side1.k = bmax.z; this->side1.material_index = this->material_index;
	}
	// side 2
	{
		this->side2.x0 = bmin.x; this->side2.x1 = bmax.x;
		this->side2.y0 = bmin.y, this->side2.y1 = bmax.y;
		this->side2.k = bmin.z; this->side2.material_index = this->material_index;
	}
	// side 3
	{
		this->side3.x0 = bmin.x; this->side3.x1 = bmax.x;
		this->side3.z0 = bmin.z, this->side3.z1 = bmax.z;
		this->side3.k = bmax.y; this->side3.material_index = this->material_index;
	}
	// side 4
	{
		this->side4.x0 = bmin.x; this->side4.x1 = bmax.x;
		this->side4.z0 = bmin.z, this->side4.z1 = bmax.z;
		this->side4.k = bmin.y; this->side4.material_index = this->material_index;
	}
	// side 5
	{
		this->side5.y0 = bmin.y; this->side5.y1 = bmax.y;
		this->side5.z0 = bmin.z, this->side5.z1 = bmax.z;
		this->side5.k = bmax.x; this->side5.material_index = this->material_index;
	}
	// side 6
	{
		this->side6.y0 = bmin.y; this->side6.y1 = bmax.y;
		this->side6.z0 = bmin.z, this->side6.z1 = bmax.z;
		this->side6.k = bmin.x; this->side6.material_index = this->material_index;
	}
}
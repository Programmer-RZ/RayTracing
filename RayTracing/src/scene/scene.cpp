#include "scene.hpp"

void Scene::createNewSphere(glm::vec3 camera_pos, glm::vec3 camera_dir) {
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
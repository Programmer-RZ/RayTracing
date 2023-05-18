#include "sceneinfo.h"

SceneInfo::SceneInfo()
	: inifile("..\\Data\\sceneInfo.ini"),
	sphere_names("..\\Data\\sphere_names.txt"),
	finishedSave(false)
{
	this->inifile.read(this->ini);
}

void SceneInfo::read() {
	
}

void SceneInfo::write(std::vector<Sphere>& spheres, std::vector<Material>& materials) {
	for (Sphere sphere : spheres) {
		// write the name to sphere_names.txt
		this->sphere_names << sphere.name << std::endl;

		// write the sphere's info to scene_info.ini
		this->ini[sphere.name]["x"]        = std::to_string(sphere.pos.x);
		this->ini[sphere.name]["y"]        = std::to_string(sphere.pos.y);
		this->ini[sphere.name]["z"]        = std::to_string(sphere.pos.z);
		this->ini[sphere.name]["radius"]   = std::to_string(sphere.radius);
		this->ini[sphere.name]["material"] = std::to_string(sphere.material_index);
	}
	this->inifile.write(this->ini);
	this->finishedSave = true;
}



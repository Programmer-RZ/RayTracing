#include "sceneinfo.h"

SceneInfo::SceneInfo()
	: inifile("..\\Data\\sceneInfo.ini"),
	sphere_names("..\\Data\\sphere_names.txt"),
	material_names("..\\Data\\material_names.txt"),
	finishedSave(false)
{
	this->inifile.read(this->ini);
}

SceneInfo::~SceneInfo() {
	this->sphere_names.close();
	this->material_names.close();
	this->clear_spherenames.close();
	this->clear_materialnames.close();
}

void SceneInfo::read(std::vector<Sphere>& spheres, std::vector<Material>& materials) {
	
}

void SceneInfo::write(std::vector<Sphere>& spheres, std::vector<Material>& materials) {
	// clear old data
	this->clear_spherenames.open("..\\Data\\sphere_names.txt", std::ofstream::out | std::ofstream::trunc);
	this->clear_materialnames.open("..\\Data\\material_names.txt", std::ofstream::out | std::ofstream::trunc);
	this->ini.clear();
	
	// spheres
	for (Sphere sphere : spheres) {
		// write the name to sphere_names.txt
		this->sphere_names << "<" << sphere.name;

		// write the sphere's info to scene_info.ini
		this->ini[sphere.name]["x"]        = std::to_string(sphere.pos.x);
		this->ini[sphere.name]["y"]        = std::to_string(sphere.pos.y);
		this->ini[sphere.name]["z"]        = std::to_string(sphere.pos.z);
		this->ini[sphere.name]["radius"]   = std::to_string(sphere.radius);
		this->ini[sphere.name]["material"] = std::to_string(sphere.material_index);
	}

	// materials
	for (Material material : materials) {
		// write the name to material_names.txt
		this->material_names << "<" << material.name;

		// write the material's info to scene_info.ini
		this->ini[material.name]["r"]         = std::to_string(material.Albedo.r);
		this->ini[material.name]["b"]         = std::to_string(material.Albedo.b);
		this->ini[material.name]["g"]         = std::to_string(material.Albedo.g);
		this->ini[material.name]["roughness"] = std::to_string(material.roughness);
	}
	this->inifile.write(this->ini, true);
	this->finishedSave = true;
}



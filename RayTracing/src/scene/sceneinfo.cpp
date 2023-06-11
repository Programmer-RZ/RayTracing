#include "sceneinfo.h"
#include <iostream>

#include "../global.h"
#include "../utils.h"

SceneInfo::SceneInfo()
	: inifile(INIFILE_PATH),
	finishedSave(false)
{
	this->inifile.read(this->ini);

	this->files = { 
		SPHERENAME_PATH, 
		MATERIALNAME_PATH, 
		SCENENAME_PATH 
	};
}

void SceneInfo::open() {
	this->sphereNames.open(SPHERENAME_PATH);
	this->materialNames.open(MATERIALNAME_PATH);
	this->sceneName.open(SCENENAME_PATH);
}

void SceneInfo::close() {
	this->sphereNames.close();
	this->materialNames.close();
	this->sceneName.close();
}

void SceneInfo::clear() {
	this->sphereNames.open(SPHERENAME_PATH, std::fstream::out | std::fstream::trunc);
	this->materialNames.open(MATERIALNAME_PATH, std::fstream::out | std::fstream::trunc);
	this->sceneName.open(SCENENAME_PATH, std::fstream::out | std::fstream::trunc);

	this->close();
}

void SceneInfo::read(Scene& scene, Camera& camera, int& width, int& height) {

	this->open();

	std::vector<std::string> sphere_names = {};
	std::vector<std::string> material_names = {};
	std::string current_name;

	// read names of spheres
	while (std::getline(this->sphereNames, current_name)) {
		// check if name is not blank
		if (!current_name.empty()) {
			sphere_names.push_back(current_name);
		}
	}

	// read names of materials
	while (std::getline(this->materialNames, current_name)) {
		// check if name is not blank
		if (!current_name.empty()) {
			material_names.push_back(current_name);
		}
	}

	// read scene name
	while (std::getline(this->sceneName, scene.name)) {
		continue;
	}

	// now use the sphere_names and material_names
	// to find their corresponding data
	// in sceneInfo.ini
	if (this->ini.size() == 0) {
		// ini is empty
		return;
	}

	for (std::string name : sphere_names) {
		// std::stod converts std::string to double
		float x      = static_cast<float>(std::stod(this->ini.get(name).get("x")));
		float y      = static_cast<float>(std::stod(this->ini.get(name).get("y")));
		float z      = static_cast<float>(std::stod(this->ini.get(name).get("z")));
		float radius = static_cast<float>(std::stod(this->ini.get(name).get("radius")));
		int material = static_cast<int>(std::stod(this->ini.get(name).get("material")));

		Sphere sphere;
		sphere.pos            = glm::vec3(x, y, z);
		sphere.name           = name;
		sphere.radius         = radius;
		sphere.material_index = material;

		scene.spheres.push_back(sphere);
	}
	for (std::string name : material_names) {
		// std::stod converts std::string to double
		float r         = static_cast<float>(std::stod(this->ini.get(name).get("r")));
		float g         = static_cast<float>(std::stod(this->ini.get(name).get("g")));
		float b         = static_cast<float>(std::stod(this->ini.get(name).get("b")));
		float roughness = static_cast<float>(std::stod(this->ini.get(name).get("roughness")));
		float emission = static_cast<float>(std::stod(this->ini.get(name).get("emission")));

		Material material;
		material.Albedo    = glm::vec3(r, g, b);
		material.roughness = roughness;
		material.EmissionPower = emission;
		material.name = name;

		scene.materials.push_back(material);
	}

	// apperance

	// skycolor
	float sc_r     = static_cast<float>(std::stod(this->ini.get("skycolor").get("r")));
	float sc_g     = static_cast<float>(std::stod(this->ini.get("skycolor").get("g")));
	float sc_b     = static_cast<float>(std::stod(this->ini.get("skycolor").get("b")));
	scene.skycolor = glm::vec3(sc_r, sc_g, sc_b);

	// settings
	width      = static_cast<int>(std::stod(this->ini.get("settings").get("width")));
	height     = static_cast<int>(std::stod(this->ini.get("settings").get("height")));
	
	// camera
	float px = static_cast<float>(std::stod(this->ini.get("camera").get("px")));
	float py = static_cast<float>(std::stod(this->ini.get("camera").get("py")));
	float pz = static_cast<float>(std::stod(this->ini.get("camera").get("pz")));
	camera.SetPosition(glm::vec3(px, py, pz));

	float dx = static_cast<float>(std::stod(this->ini.get("camera").get("dx")));
	float dy = static_cast<float>(std::stod(this->ini.get("camera").get("dy")));
	float dz = static_cast<float>(std::stod(this->ini.get("camera").get("dz")));
	camera.SetDirection(glm::vec3(dx, dy, dz));

	camera.SetVerticalFOV(static_cast<float>(std::stod(this->ini.get("camera").get("verticalFOV"))));
	camera.SetNearClip(static_cast<float>(std::stod(this->ini.get("camera").get("nearClip"))));
	camera.SetFarClip(static_cast<float>(std::stod(this->ini.get("camera").get("farClip"))));

	camera.RecalculateView();
	camera.RecalculateRayDirections();

	this->close();
}

void SceneInfo::write(Scene& scene, Camera& camera, int width, int height) {
	// clear old data
	this->clear();

	this->open();
	
	// spheres
	for (Sphere sphere : scene.spheres) {
		// write the name to sphere_names.txt
		this->sphereNames << sphere.name << std::endl;

		// write the sphere's info to scene_info.ini
		this->ini[sphere.name]["x"]        = std::to_string(sphere.pos.x);
		this->ini[sphere.name]["y"]        = std::to_string(sphere.pos.y);
		this->ini[sphere.name]["z"]        = std::to_string(sphere.pos.z);
		this->ini[sphere.name]["radius"]   = std::to_string(sphere.radius);
		this->ini[sphere.name]["material"] = std::to_string(sphere.material_index);
	}

	// materials
	for (Material material : scene.materials) {
		// write the name to material_names.txt
		this->materialNames << material.name << std::endl;

		// write the material's info to scene_info.ini
		this->ini[material.name]["r"]         = std::to_string(material.Albedo.r);
		this->ini[material.name]["b"]         = std::to_string(material.Albedo.b);
		this->ini[material.name]["g"]         = std::to_string(material.Albedo.g);
		this->ini[material.name]["roughness"] = std::to_string(material.roughness);
		this->ini[material.name]["emission"] = std::to_string(material.EmissionPower);
	}

	// scene name
	this->sceneName << scene.name;

	// appearance

	this->ini["skycolor"]["r"] = std::to_string(scene.skycolor.r);
	this->ini["skycolor"]["g"] = std::to_string(scene.skycolor.g);
	this->ini["skycolor"]["b"] = std::to_string(scene.skycolor.b);

	// settings
	this->ini["settings"]["width"]      = std::to_string(width);
	this->ini["settings"]["height"]     = std::to_string(height);

	// camera
	this->ini["camera"]["px"] = std::to_string(camera.GetPosition().x);
	this->ini["camera"]["py"] = std::to_string(camera.GetPosition().y);
	this->ini["camera"]["pz"] = std::to_string(camera.GetPosition().z);
	
	this->ini["camera"]["dx"] = std::to_string(camera.GetDirection().x);
	this->ini["camera"]["dy"] = std::to_string(camera.GetDirection().y);
	this->ini["camera"]["dz"] = std::to_string(camera.GetDirection().z);

	this->ini["camera"]["verticalFOV"] = std::to_string(camera.GetVerticalFOV());
	this->ini["camera"]["nearClip"]    = std::to_string(camera.GetNearClip());
	this->ini["camera"]["farClip"]     = std::to_string(camera.GetFarClip());
	
	// write all the contents
	this->inifile.write(this->ini, true);
	this->finishedSave = true;

	this->close();
}



#include "sceneinfo.h"
#include <iostream>

#include "../global.h"

SceneInfo::SceneInfo()
	: inifile(INIFILE_PATH),
	finishedSave(false)
{
	this->inifile.read(this->ini);
}
SceneInfo::~SceneInfo() {
	this->close();
}

void SceneInfo::reopen() {
	this->sphere_names.open(SPHERENAME_PATH);
	this->material_names.open(MATERIALNAME_PATH);
	this->sceneName.open(SCENENAME_PATH);
}

void SceneInfo::clear() {
	this->sphere_names.open(SPHERENAME_PATH, std::ofstream::out | std::ofstream::trunc);
	this->material_names.open(MATERIALNAME_PATH, std::ofstream::out | std::ofstream::trunc);
	this->sceneName.open(SCENENAME_PATH, std::ofstream::out | std::ofstream::trunc);
	this->ini.clear();

	this->close();
}
void SceneInfo::close() {
	this->sphere_names.close();
	this->material_names.close();
	this->sceneName.close();
}

void SceneInfo::read(Scene& scene, Camera& camera, int& width, int& height, float& brightness) {

	this->reopen();

	std::vector<std::string> sphere_names = {};
	std::vector<std::string> material_names = {};
	std::string current_name;

	// read names of spheres
	while (std::getline(this->sphere_names, current_name)) {
		// check if name is not blank
		if (!current_name.empty()) {
			sphere_names.push_back(current_name);
		}
	}

	// read names of materials
	while (std::getline(this->material_names, current_name)) {
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

		Material material;
		material.Albedo    = glm::vec3(r, g, b);
		material.roughness = roughness;

		scene.materials.push_back(material);
	}

	// apperance
	// lightdir
	float ld_x     = static_cast<float>(std::stod(this->ini.get("lightdir").get("x")));
	float ld_y     = static_cast<float>(std::stod(this->ini.get("lightdir").get("y")));
	float ld_z     = static_cast<float>(std::stod(this->ini.get("lightdir").get("z")));
	scene.lightDir = glm::vec3(ld_x, ld_y, ld_z);

	// skycolor
	float sc_r     = static_cast<float>(std::stod(this->ini.get("skycolor").get("r")));
	float sc_g     = static_cast<float>(std::stod(this->ini.get("skycolor").get("g")));
	float sc_b     = static_cast<float>(std::stod(this->ini.get("skycolor").get("b")));
	scene.skycolor = glm::vec3(sc_r, sc_g, sc_b);

	// settings
	width      = static_cast<int>(std::stod(this->ini.get("settings").get("width")));
	height     = static_cast<int>(std::stod(this->ini.get("settings").get("height")));
	brightness = static_cast<float>(std::stod(this->ini.get("settings").get("brightness")));
	
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

void SceneInfo::write(Scene& scene, Camera& camera, int width, int height, float brightness) {
	// clear old data
	this->clear();

	this->reopen();
	
	// spheres
	for (Sphere sphere : scene.spheres) {
		// write the name to sphere_names.txt
		this->sphere_names << sphere.name << std::endl;

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
		this->material_names << material.name << std::endl;

		// write the material's info to scene_info.ini
		this->ini[material.name]["r"]         = std::to_string(material.Albedo.r);
		this->ini[material.name]["b"]         = std::to_string(material.Albedo.b);
		this->ini[material.name]["g"]         = std::to_string(material.Albedo.g);
		this->ini[material.name]["roughness"] = std::to_string(material.roughness);
	}

	// scene name
	this->sceneName << scene.name;

	// appearance
	this->ini["lightdir"]["x"] = std::to_string(scene.lightDir.x);
	this->ini["lightdir"]["y"] = std::to_string(scene.lightDir.y);
	this->ini["lightdir"]["z"] = std::to_string(scene.lightDir.z);

	this->ini["skycolor"]["r"] = std::to_string(scene.skycolor.r);
	this->ini["skycolor"]["g"] = std::to_string(scene.skycolor.g);
	this->ini["skycolor"]["b"] = std::to_string(scene.skycolor.b);

	// settings
	this->ini["settings"]["width"]      = std::to_string(width);
	this->ini["settings"]["height"]     = std::to_string(height);
	this->ini["settings"]["brightness"] = std::to_string(brightness);

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



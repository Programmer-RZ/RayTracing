#include "sceneinfo.hpp"
#include <iostream>

#include "../global.h"
#include "../utils.hpp"

SceneInfo::SceneInfo()
	: scenefile(SCENEINFO),
	spherefile(SPHEREINFO),
	boxfile(BOXINFO),
	materialfile(MATERIALINFO)
{
	this->scenefile.read(this->sceneini);
	this->spherefile.read(this->sphereini);
	this->boxfile.read(this->boxini);
	this->materialfile.read(this->materialini);
}

void SceneInfo::clear() {
	this->sceneini.clear();
	this->sphereini.clear();
	this->boxini.clear();
	this->materialini.clear();
}

void SceneInfo::read(Scene& scene, Camera& camera, int& width, int& height) {

	for (auto s : this->sphereini) {
		float x = static_cast<float>(std::stod(s.second.get("x")));
		float y = static_cast<float>(std::stod(s.second.get("y")));
		float z = static_cast<float>(std::stod(s.second.get("z")));
		float radius = static_cast<float>(std::stod(s.second.get("radius")));
		int material = static_cast<int>(std::stod(s.second.get("material")));

		Sphere sphere = Sphere(glm::vec3(x, y, z), radius, s.first, material);
		scene.spheres.push_back(sphere);
	}
	
	for (auto b : this->boxini) {
		glm::vec3 boxmin = {
			static_cast<float>(std::stod(b.second.get("boxminx"))),
			static_cast<float>(std::stod(b.second.get("boxminy"))),
			static_cast<float>(std::stod(b.second.get("boxminz"))),
		};
		glm::vec3 boxmax = {
			static_cast<float>(std::stod(b.second.get("boxmaxx"))),
			static_cast<float>(std::stod(b.second.get("boxmaxy"))),
			static_cast<float>(std::stod(b.second.get("boxmaxz"))),
		};
		
		int material = static_cast<int>(std::stod(b.second.get("material")));

		Box box = Box(boxmin, boxmax, b.first, material);
		scene.boxes.push_back(box);
	}

	
	for (auto m : this->materialini) {
		int id = std::stoi(m.second.get("id"));
		float r = static_cast<float>(std::stod(m.second.get("r")));
		float g = static_cast<float>(std::stod(m.second.get("g")));
		float b = static_cast<float>(std::stod(m.second.get("b")));
		float roughness = static_cast<float>(std::stod(m.second.get("roughness")));
		float emission = static_cast<float>(std::stod(m.second.get("emission")));
		std::string lighting = m.second.get("lighting");

		Material material = Material(glm::vec3{r, g, b}, roughness, emission, id, lighting, m.first);

		scene.materials.push_back(material);
	}

	// apperance

	// skycolor
	float sc_r     = static_cast<float>(std::stod(this->sceneini.get("skycolor").get("r")));
	float sc_g     = static_cast<float>(std::stod(this->sceneini.get("skycolor").get("g")));
	float sc_b     = static_cast<float>(std::stod(this->sceneini.get("skycolor").get("b")));
	scene.skycolor = glm::vec3(sc_r, sc_g, sc_b);

	// settings
	width      = static_cast<int>(std::stod(this->sceneini.get("settings").get("width")));
	height     = static_cast<int>(std::stod(this->sceneini.get("settings").get("height")));
	
	// camera
	float px = static_cast<float>(std::stod(this->sceneini.get("camera").get("px")));
	float py = static_cast<float>(std::stod(this->sceneini.get("camera").get("py")));
	float pz = static_cast<float>(std::stod(this->sceneini.get("camera").get("pz")));
	camera.SetPosition(glm::vec3(px, py, pz));

	float dx = static_cast<float>(std::stod(this->sceneini.get("camera").get("dx")));
	float dy = static_cast<float>(std::stod(this->sceneini.get("camera").get("dy")));
	float dz = static_cast<float>(std::stod(this->sceneini.get("camera").get("dz")));
	camera.SetDirection(glm::vec3(dx, dy, dz));

	camera.SetVerticalFOV(static_cast<float>(std::stod(this->sceneini.get("camera").get("verticalFOV"))));
	camera.SetNearClip(static_cast<float>(std::stod(this->sceneini.get("camera").get("nearClip"))));
	camera.SetFarClip(static_cast<float>(std::stod(this->sceneini.get("camera").get("farClip"))));

	camera.RecalculateView();
}

void SceneInfo::write(Scene& scene, Camera& camera, int width, int height) {
	// clear old data
	this->clear();
	
	// spheres
	for (Sphere sphere : scene.spheres) {
		// write the sphere's info to sphereInfo.ini
		this->sphereini[sphere.name]["x"]        = std::to_string(sphere.pos.x);
		this->sphereini[sphere.name]["y"]        = std::to_string(sphere.pos.y);
		this->sphereini[sphere.name]["z"]        = std::to_string(sphere.pos.z);
		this->sphereini[sphere.name]["radius"]   = std::to_string(sphere.radius);
		this->sphereini[sphere.name]["material"] = std::to_string(sphere.material_index);
	}
	
	// boxes
	for (Box box : scene.boxes) {
		// write the box's info to boxInfo.ini
		this->boxini[box.name]["boxminx"]  = std::to_string(box.box_min.x);
		this->boxini[box.name]["boxminy"]  = std::to_string(box.box_min.y);
		this->boxini[box.name]["boxminz"]  = std::to_string(box.box_min.z);
		this->boxini[box.name]["boxmaxx"]  = std::to_string(box.box_max.x);
		this->boxini[box.name]["boxmaxy"]  = std::to_string(box.box_max.y);
		this->boxini[box.name]["boxmaxz"]  = std::to_string(box.box_max.z);
		this->boxini[box.name]["material"] = std::to_string(box.material_index);
	}

	// materials
	for (Material material : scene.materials) {
		// write the material's info to materialInfo.ini
		this->materialini[material.name]["id"] 		  = std::to_string(material.id);
		this->materialini[material.name]["r"]         = std::to_string(material.Albedo.r);
		this->materialini[material.name]["b"]         = std::to_string(material.Albedo.b);
		this->materialini[material.name]["g"]         = std::to_string(material.Albedo.g);
		this->materialini[material.name]["roughness"] = std::to_string(material.roughness);
		this->materialini[material.name]["emission"] = std::to_string(material.EmissionPower);
		this->materialini[material.name]["lighting"] = material.lighting;
	}

	// appearance

	this->sceneini["skycolor"]["r"] = std::to_string(scene.skycolor.r);
	this->sceneini["skycolor"]["g"] = std::to_string(scene.skycolor.g);
	this->sceneini["skycolor"]["b"] = std::to_string(scene.skycolor.b);

	// settings
	this->sceneini["settings"]["width"]      = std::to_string(width);
	this->sceneini["settings"]["height"]     = std::to_string(height);

	// camera
	this->sceneini["camera"]["px"] = std::to_string(camera.GetPosition().x);
	this->sceneini["camera"]["py"] = std::to_string(camera.GetPosition().y);
	this->sceneini["camera"]["pz"] = std::to_string(camera.GetPosition().z);
	
	this->sceneini["camera"]["dx"] = std::to_string(camera.GetDirection().x);
	this->sceneini["camera"]["dy"] = std::to_string(camera.GetDirection().y);
	this->sceneini["camera"]["dz"] = std::to_string(camera.GetDirection().z);

	this->sceneini["camera"]["verticalFOV"] = std::to_string(camera.GetVerticalFOV());
	this->sceneini["camera"]["nearClip"]    = std::to_string(camera.GetNearClip());
	this->sceneini["camera"]["farClip"]     = std::to_string(camera.GetFarClip());
	
	// write all the contents
	this->scenefile.write(this->sceneini, true);
	this->spherefile.write(this->sphereini, true);
	this->boxfile.write(this->boxini, true);
	this->materialfile.write(this->materialini, true);
}



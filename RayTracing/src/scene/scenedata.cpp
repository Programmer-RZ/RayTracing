#include "scenedata.hpp"

#include "../global.h"
#include "../utils.hpp"

SceneData::SceneData()
	: sceneDataFileName("sceneData.ini"),
	sphereDataFileName("sphereData.ini"),
	boxDataFileName("boxData.ini"),
	materialDataFileName("materialData.ini")
{}

void SceneData::read(Scene& scene, Camera& camera, int& width, int& height) {

	nfdchar_t* outpath = nullptr;
	nfdresult_t result = NFD_PickFolder(NULL, &(outpath));

	if (result == NFD_CANCEL || result == NFD_ERROR) {
		return;
	}

	this->path = std::filesystem::path(outpath);

	// full paths
	std::filesystem::path SceneDataPath = this->path / this->sceneDataFileName;
	std::filesystem::path SphereDataPath = this->path / this->sphereDataFileName;
	std::filesystem::path BoxDataPath = this->path / this->boxDataFileName;
	std::filesystem::path MaterialDataPath = this->path / this->materialDataFileName;

	mINI::INIFile scenefile(SceneDataPath.string());
	mINI::INIStructure sceneini;
	scenefile.read(sceneini);

	mINI::INIFile spherefile(SphereDataPath.string());
	mINI::INIStructure sphereini;
	spherefile.read(sphereini);

	mINI::INIFile boxfile(BoxDataPath.string());
	mINI::INIStructure boxini;
	boxfile.read(boxini);

	mINI::INIFile materialfile(MaterialDataPath.string());
	mINI::INIStructure materialini;
	materialfile.read(materialini);

	// spheres
	scene.spheres = {};
	for (auto s : sphereini) {
		float x = static_cast<float>(std::stod(s.second.get("x")));
		float y = static_cast<float>(std::stod(s.second.get("y")));
		float z = static_cast<float>(std::stod(s.second.get("z")));
		float radius = static_cast<float>(std::stod(s.second.get("radius")));
		int material = static_cast<int>(std::stod(s.second.get("material")));

		Sphere sphere = Sphere(glm::vec3(x, y, z), radius, s.first, material);
		scene.spheres.push_back(sphere);
	}
	
	// boxes
	scene.boxes = {};
	for (auto b : boxini) {
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

	// materials
	scene.materials = {};
	for (auto m : materialini) {
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

	// skycolor
	float sc_r     = static_cast<float>(std::stod(sceneini.get("skycolor").get("r")));
	float sc_g     = static_cast<float>(std::stod(sceneini.get("skycolor").get("g")));
	float sc_b     = static_cast<float>(std::stod(sceneini.get("skycolor").get("b")));
	scene.skycolor = glm::vec3(sc_r, sc_g, sc_b);

	// settings
	width      = static_cast<int>(std::stod(sceneini.get("settings").get("width")));
	height     = static_cast<int>(std::stod(sceneini.get("settings").get("height")));
	scene.name = sceneini.get("settings").get("name");
	
	// camera
	float px = static_cast<float>(std::stod(sceneini.get("camera").get("px")));
	float py = static_cast<float>(std::stod(sceneini.get("camera").get("py")));
	float pz = static_cast<float>(std::stod(sceneini.get("camera").get("pz")));
	camera.SetPosition(glm::vec3(px, py, pz));

	float dx = static_cast<float>(std::stod(sceneini.get("camera").get("dx")));
	float dy = static_cast<float>(std::stod(sceneini.get("camera").get("dy")));
	float dz = static_cast<float>(std::stod(sceneini.get("camera").get("dz")));
	camera.SetDirection(glm::vec3(dx, dy, dz));

	camera.SetVerticalFOV(static_cast<float>(std::stod(sceneini.get("camera").get("verticalFOV"))));
	camera.SetNearClip(static_cast<float>(std::stod(sceneini.get("camera").get("nearClip"))));
	camera.SetFarClip(static_cast<float>(std::stod(sceneini.get("camera").get("farClip"))));

	camera.RecalculateView();
}

void SceneData::write(Scene& scene, Camera& camera, int width, int height) {

	if (this->path.string() != "") {
		// save
		this->save(scene, camera, width, height);
	}
	else {
		// saveas
		this->saveas(scene, camera, width, height);
	}
	
}

void SceneData::save(Scene& scene, Camera& camera, int width, int height) {
	// full paths
	std::filesystem::path SceneDataPath = this->path / this->sceneDataFileName;
	std::filesystem::path SphereDataPath = this->path / this->sphereDataFileName;
	std::filesystem::path BoxDataPath = this->path / this->boxDataFileName;
	std::filesystem::path MaterialDataPath = this->path / this->materialDataFileName;

	mINI::INIFile scenefile(SceneDataPath.string());
	mINI::INIStructure sceneini;

	mINI::INIFile spherefile(SphereDataPath.string());
	mINI::INIStructure sphereini;

	mINI::INIFile boxfile(BoxDataPath.string());
	mINI::INIStructure boxini;

	mINI::INIFile materialfile(MaterialDataPath.string());
	mINI::INIStructure materialini;

	// clear old data
	sceneini.clear(); sphereini.clear(); boxini.clear(); materialini.clear();

	// spheres
	for (Sphere sphere : scene.spheres) {
		// write the sphere's info to sphereInfo.ini
		sphereini[sphere.name]["x"] = std::to_string(sphere.pos.x);
		sphereini[sphere.name]["y"] = std::to_string(sphere.pos.y);
		sphereini[sphere.name]["z"] = std::to_string(sphere.pos.z);
		sphereini[sphere.name]["radius"] = std::to_string(sphere.radius);
		sphereini[sphere.name]["material"] = std::to_string(sphere.material_index);
	}

	// boxes
	for (Box box : scene.boxes) {
		// write the box's info to boxInfo.ini
		boxini[box.name]["boxminx"] = std::to_string(box.box_min.x);
		boxini[box.name]["boxminy"] = std::to_string(box.box_min.y);
		boxini[box.name]["boxminz"] = std::to_string(box.box_min.z);
		boxini[box.name]["boxmaxx"] = std::to_string(box.box_max.x);
		boxini[box.name]["boxmaxy"] = std::to_string(box.box_max.y);
		boxini[box.name]["boxmaxz"] = std::to_string(box.box_max.z);
		boxini[box.name]["material"] = std::to_string(box.material_index);
	}

	// materials
	for (Material material : scene.materials) {
		// write the material's info to materialInfo.ini
		materialini[material.name]["id"] = std::to_string(material.id);
		materialini[material.name]["r"] = std::to_string(material.Albedo.r);
		materialini[material.name]["b"] = std::to_string(material.Albedo.b);
		materialini[material.name]["g"] = std::to_string(material.Albedo.g);
		materialini[material.name]["roughness"] = std::to_string(material.roughness);
		materialini[material.name]["emission"] = std::to_string(material.EmissionPower);
		materialini[material.name]["lighting"] = material.lighting;
	}

	// appearance

	sceneini["skycolor"]["r"] = std::to_string(scene.skycolor.r);
	sceneini["skycolor"]["g"] = std::to_string(scene.skycolor.g);
	sceneini["skycolor"]["b"] = std::to_string(scene.skycolor.b);

	// settings
	sceneini["settings"]["width"] = std::to_string(width);
	sceneini["settings"]["height"] = std::to_string(height);
	sceneini["settings"]["name"] = scene.name;

	// camera
	sceneini["camera"]["px"] = std::to_string(camera.GetPosition().x);
	sceneini["camera"]["py"] = std::to_string(camera.GetPosition().y);
	sceneini["camera"]["pz"] = std::to_string(camera.GetPosition().z);

	sceneini["camera"]["dx"] = std::to_string(camera.GetDirection().x);
	sceneini["camera"]["dy"] = std::to_string(camera.GetDirection().y);
	sceneini["camera"]["dz"] = std::to_string(camera.GetDirection().z);

	sceneini["camera"]["verticalFOV"] = std::to_string(camera.GetVerticalFOV());
	sceneini["camera"]["nearClip"] = std::to_string(camera.GetNearClip());
	sceneini["camera"]["farClip"] = std::to_string(camera.GetFarClip());

	// write all the contents
	scenefile.write(sceneini, true);
	spherefile.write(sphereini, true);
	boxfile.write(boxini, true);
	materialfile.write(materialini, true);
}

void SceneData::saveas(Scene& scene, Camera& camera, int width, int height) {
	nfdchar_t* parentdirectory = nullptr;
	nfdresult_t result = NFD_PickFolder(NULL, &(parentdirectory));

	if (result == NFD_CANCEL || result == NFD_ERROR) {
		return;
	}

	this->path = std::filesystem::path(parentdirectory) / std::filesystem::path(scene.name);

	// create the directory
	std::filesystem::create_directory(this->path);

	// create the files
	std::ofstream newSceneDataFile(this->path / this->sceneDataFileName); newSceneDataFile.close();
	std::ofstream newSphereDataFile(this->path / this->sphereDataFileName); newSphereDataFile.close();
	std::ofstream newBoxDataFile(this->path / this->boxDataFileName); newBoxDataFile.close();
	std::ofstream newMaterialDataFile(this->path / this->materialDataFileName); newMaterialDataFile.close();

	// save
	this->save(scene, camera, width, height);
}


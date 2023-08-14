#include "scenedata.hpp"

#include "../utils.hpp"

void SceneData::read(Scene& scene, Camera& camera, int& width, int& height) {

	nfdchar_t* outpath = nullptr;
	nfdresult_t result = NFD_OpenDialog("ini", NULL, &(outpath));

	if (result == NFD_CANCEL || result == NFD_ERROR) {
		return;
	}

	this->filepath = outpath;

	mINI::INIFile scenefile(this->filepath);
	mINI::INIStructure sceneini;
	scenefile.read(sceneini);

	// reset all scene vectors
	scene.spheres = {};
	scene.boxes = {};
	scene.materials = {};

	// iterate through all the pairs in sceneini
	for (auto p : sceneini) {
		// check type
		std::string type = p.second.get("type");

		if (type == "sphere") {
			float x = static_cast<float>(std::stod(p.second.get("x")));
			float y = static_cast<float>(std::stod(p.second.get("y")));
			float z = static_cast<float>(std::stod(p.second.get("z")));
			float radius = static_cast<float>(std::stod(p.second.get("radius")));
			size_t material = static_cast<size_t>(std::stod(p.second.get("material")));

			Sphere sphere = Sphere(glm::vec3(x, y, z), radius, p.first, material);
			scene.spheres.push_back(sphere);
		}

		else if (type == "box") {
			glm::vec3 boxmin = {
			static_cast<float>(std::stod(p.second.get("boxminx"))),
			static_cast<float>(std::stod(p.second.get("boxminy"))),
			static_cast<float>(std::stod(p.second.get("boxminz"))),
			};
			glm::vec3 boxmax = {
				static_cast<float>(std::stod(p.second.get("boxmaxx"))),
				static_cast<float>(std::stod(p.second.get("boxmaxy"))),
				static_cast<float>(std::stod(p.second.get("boxmaxz"))),
			};

			size_t material = static_cast<size_t>(std::stod(p.second.get("material")));

			Box box = Box(boxmin, boxmax, p.first, material);
			scene.boxes.push_back(box);
		}

		else if (type == "material") {
			int id = std::stoi(p.second.get("id"));
			float r = static_cast<float>(std::stod(p.second.get("r")));
			float g = static_cast<float>(std::stod(p.second.get("g")));
			float b = static_cast<float>(std::stod(p.second.get("b")));
			float roughness = static_cast<float>(std::stod(p.second.get("roughness")));
			float emission = static_cast<float>(std::stod(p.second.get("emission")));
			std::string materialtype = p.second.get("materialtype");

			Material material = Material(glm::vec3{r, g, b}, roughness, emission, id, materialtype, p.first);

			scene.materials.push_back(material);
		}

		else if (type == "skycolor") {
			float sc_r = static_cast<float>(std::stod(p.second.get("r")));
			float sc_g = static_cast<float>(std::stod(p.second.get("g")));
			float sc_b = static_cast<float>(std::stod(p.second.get("b")));
			scene.skycolor = glm::vec3(sc_r, sc_g, sc_b);
		}

		else if (type == "settings") {
			width = static_cast<int>(std::stod(p.second.get("width")));
			height = static_cast<int>(std::stod(p.second.get("height")));
			scene.name = p.second.get("name");
		}

		else if (type == "camera") {
			float px = static_cast<float>(std::stod(p.second.get("px")));
			float py = static_cast<float>(std::stod(p.second.get("py")));
			float pz = static_cast<float>(std::stod(p.second.get("pz")));
			camera.SetPosition(glm::vec3(px, py, pz));

			float dx = static_cast<float>(std::stod(p.second.get("dx")));
			float dy = static_cast<float>(std::stod(p.second.get("dy")));
			float dz = static_cast<float>(std::stod(p.second.get("dz")));
			camera.SetDirection(glm::vec3(dx, dy, dz));

			camera.SetVerticalFOV(static_cast<float>(std::stod(p.second.get("verticalFOV"))));
			camera.SetNearClip(static_cast<float>(std::stod(p.second.get("nearClip"))));
			camera.SetFarClip(static_cast<float>(std::stod(p.second.get("farClip"))));

			camera.RecalculateView();
		}
	}
}

void SceneData::write(const Scene& scene, const Camera& camera, const int width, const int height) {

	if (this->filepath != "") {
		// save
		this->save(scene, camera, width, height);
	}
	else {
		// saveas
		this->saveas(scene, camera, width, height);
	}
	
}

void SceneData::save(const Scene& scene, const Camera& camera, const int width, const int height) {

	mINI::INIFile scenefile(this->filepath);
	mINI::INIStructure sceneini;

	// clear old data
	sceneini.clear();

	// spheres
	for (Sphere sphere : scene.spheres) {
		// write the sphere's info
		sceneini[sphere.name]["type"] = "sphere";

		sceneini[sphere.name]["x"] = std::to_string(sphere.pos.x);
		sceneini[sphere.name]["y"] = std::to_string(sphere.pos.y);
		sceneini[sphere.name]["z"] = std::to_string(sphere.pos.z);
		sceneini[sphere.name]["radius"] = std::to_string(sphere.radius);
		sceneini[sphere.name]["material"] = std::to_string(sphere.material_index);
	}

	// boxes
	for (Box box : scene.boxes) {
		// write the box's info
		sceneini[box.name]["type"] = "box";

		sceneini[box.name]["boxminx"] = std::to_string(box.box_min.x);
		sceneini[box.name]["boxminy"] = std::to_string(box.box_min.y);
		sceneini[box.name]["boxminz"] = std::to_string(box.box_min.z);
		sceneini[box.name]["boxmaxx"] = std::to_string(box.box_max.x);
		sceneini[box.name]["boxmaxy"] = std::to_string(box.box_max.y);
		sceneini[box.name]["boxmaxz"] = std::to_string(box.box_max.z);
		sceneini[box.name]["material"] = std::to_string(box.material_index);
	}

	// materials
	for (Material material : scene.materials) {
		// write the material's info to materialInfo.ini
		sceneini[material.name]["type"] = "material";

		sceneini[material.name]["id"] = std::to_string(material.id);

		// color
		sceneini[material.name]["r"] = std::to_string(material.Albedo.r);
		sceneini[material.name]["b"] = std::to_string(material.Albedo.b);
		sceneini[material.name]["g"] = std::to_string(material.Albedo.g);

		sceneini[material.name]["roughness"] = std::to_string(material.roughness);
		sceneini[material.name]["emission"] = std::to_string(material.EmissionPower);

		// type
		sceneini[material.name]["materialtype"] = material.type;
	}

	// skycolor

	sceneini["skycolor"]["type"] = "skycolor";
	sceneini["skycolor"]["r"] = std::to_string(scene.skycolor.r);
	sceneini["skycolor"]["g"] = std::to_string(scene.skycolor.g);
	sceneini["skycolor"]["b"] = std::to_string(scene.skycolor.b);

	// settings
	sceneini["settings"]["type"] = "settings";
	sceneini["settings"]["width"] = std::to_string(width);
	sceneini["settings"]["height"] = std::to_string(height);
	sceneini["settings"]["name"] = scene.name;

	// camera
	sceneini["camera"]["type"] = "camera";

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
}

void SceneData::saveas(const Scene& scene, const Camera& camera, const int width, const int height) {
	nfdchar_t* outpath = nullptr;
	nfdresult_t result = NFD_SaveDialog("ini", NULL, &(outpath));

	if (result == NFD_CANCEL || result == NFD_ERROR) {
		return;
	}

	this->filepath = outpath;

	// create the files
	std::ofstream newSceneDataFile(this->filepath); newSceneDataFile.close();

	// save
	this->save(scene, camera, width, height);
}


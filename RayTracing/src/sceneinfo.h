#pragma once

#include <fstream>
#include "libs\mINI\src\mini\ini.h"
#include "scene.h"

class SceneInfo {
	std::fstream sphere_names;
	std::fstream material_names;
	
	mINI::INIFile inifile;
	mINI::INIStructure ini;

	// files for clearing all old data
	std::ofstream clear_spherenames;
	std::ofstream clear_materialnames;

	bool finishedSave;

public:
	SceneInfo();
	~SceneInfo();

	void read(std::vector<Sphere>& spheres, std::vector<Material>& materials);
	void write(std::vector<Sphere>& spheres, std::vector<Material>& materials);

	bool GetFinishedSave() const { return this->finishedSave; }
	void SetFinishedSave(bool finished) { this->finishedSave = finished; }
};

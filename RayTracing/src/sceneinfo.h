#pragma once

#include <fstream>
#include "libs\mINI\src\mini\ini.h"
#include "scene.h"

class SceneInfo {
	std::fstream sphere_names;
	mINI::INIFile inifile;
	mINI::INIStructure ini;

	bool finishedSave;

public:
	SceneInfo();

	void read();
	void write(std::vector<Sphere>& spheres, std::vector<Material>& materials);

	bool GetFinishedSave() const { return this->finishedSave; }
	void SetFinishedSave(bool finished) { this->finishedSave = finished; }
};

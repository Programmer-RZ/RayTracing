#pragma once

#include <fstream>
#include "libs\mINI\src\mini\ini.h"
#include "scene.h"

class SceneInfo {
	std::fstream sphere_names;
	std::fstream material_names;
	
	mINI::INIFile inifile;
	mINI::INIStructure ini;

	bool finishedSave;

private:
	void reopen();
	void clear();
	void close();

public:
	SceneInfo();
	~SceneInfo();

	void read(Scene& scene, int& width, int& height, float& brightness);
	void write(Scene& scene, int width, int height, float brightness);

	bool GetFinishedSave() const { return this->finishedSave; }
	void SetFinishedSave(bool finished) { this->finishedSave = finished; }
};

#pragma once

#include <fstream>
#include "..\libs\mINI\src\mini\ini.h"
#include "scene.h"
#include "../renderer/camera.h"

class SceneInfo {
private:	
	mINI::INIFile inifile;
	mINI::INIStructure ini;

	std::fstream sphereNames;
	std::fstream materialNames;
	std::fstream sceneName;

	bool finishedSave;

	std::vector<std::string> files;

private:
	void open();
	void close();
	void clear();

public:
	SceneInfo();

	void read(Scene& scene, Camera& camera, int& width, int& height, float& brightness);
	void write(Scene& scene, Camera& camera, int width, int height, float brightness);

	bool GetFinishedSave() const { return this->finishedSave; }
	void SetFinishedSave(bool finished) { this->finishedSave = finished; }
};

#pragma once

#include <fstream>
#include "..\libs\mINI\src\mini\ini.h"
#include "scene.hpp"
#include "../renderer/camera.hpp"

class SceneInfo {
private:	
	mINI::INIFile scenefile;
	mINI::INIStructure sceneini;

	mINI::INIFile spherefile;
	mINI::INIStructure sphereini;

	mINI::INIFile materialfile;
	mINI::INIStructure materialini;
	
	mINI::INIFile boxfile;
	mINI::INIStructure boxini;

private:
	void clear();

public:
	SceneInfo();

	void read(Scene& scene, Camera& camera, int& width, int& height);
	void write(Scene& scene, Camera& camera, int width, int height);
};

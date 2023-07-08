#pragma once

#include <fstream>
#include <filesystem>

#include "..\libs\mINI\src\mini\ini.h"
#include "scene.hpp"
#include "../renderer/camera.hpp"

// file dialog
#include "nfd.h"

class SceneData {
private:
	/*
	mINI::INIFile scenefile;
	mINI::INIStructure sceneini;

	mINI::INIFile spherefile;
	mINI::INIStructure sphereini;

	mINI::INIFile materialfile;
	mINI::INIStructure materialini;
	
	mINI::INIFile boxfile;
	mINI::INIStructure boxini;
	*/

	std::filesystem::path path;

	std::filesystem::path sceneDataFileName;
	std::filesystem::path sphereDataFileName;
	std::filesystem::path boxDataFileName;
	std::filesystem::path materialDataFileName;

private:
	void save(Scene& scene, Camera& camera, int width, int height);
	void saveas(Scene& scene, Camera& camera, int width, int height);

public:
	SceneData();

	void read(Scene& scene, Camera& camera, int& width, int& height);
	void write(Scene& scene, Camera& camera, int width, int height);
};

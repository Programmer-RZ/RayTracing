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
	void save(const Scene& scene, const Camera& camera, const int width, const int height);
	void saveas(const Scene& scene, const Camera& camera, const int width, const int height);

public:
	SceneData();

	// read data from the app and write to file
	void read(Scene& scene, Camera& camera, int& width, int& height);

	// take data and write it to the app
	void write(const Scene& scene, const Camera& camera, const int width, const int height);
};

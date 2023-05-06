#pragma once

#include <glm/glm.hpp>
#include <fstream>
#include <string>

#include "utils.h"

class Export {
private:
	int y_index;
	int x_index;
	bool finishedWrite;
	bool finishedExport;
	bool isExport;
	std::ofstream data;

	int percentage;

public:
	Export();

	bool GetFinishedExport() const { return this->finishedExport; }
	bool GetIsExport() const { return this->isExport; }
	int GetPercentage(int imageWidth, int imageHeight) const { return this->percentage; }
	void SetIsExport(bool isexport) { this->isExport = isexport; }

	void updatePercentage();

	void reset();

	void writeArray(std::string name, uint32_t* imageData, int imageWidth, int imageHeight);

	void ExportImage(std::string name, uint32_t* imageData, int imageWidth, int imageHeight);
};

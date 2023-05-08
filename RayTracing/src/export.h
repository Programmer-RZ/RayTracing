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

	double percentage;

public:
	Export();
	~Export();

	bool GetFinishedExport() const { return this->finishedExport; }
	bool GetIsExport() const { return this->isExport; }
	double GetPercentage() const { return this->percentage; }
	void SetIsExport(bool isexport) { this->isExport = isexport; }

	void updatePercentage(int imageWidth, int imageHeight);

	void reset();

	void writeArray(uint32_t* imageData, int imageWidth, int imageHeight);

	void ExportImage(uint32_t* imageData, int imageWidth, int imageHeight);
};

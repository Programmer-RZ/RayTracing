#pragma once

#include <glm/glm.hpp>
#include <fstream>
#include <string>

#include "../utils.h"

class Export {
private:
	int y_index;
	int x_index;
	bool finishedWrite;
	bool finishedExport;
	bool isExport;
	std::ofstream data;

	double percentage;

	char* formats[2];
	char* currentFormat;

public:
	Export();
	~Export();

	bool GetFinishedExport() const { return this->finishedExport; }
	bool GetIsExport() const { return this->isExport; }
	double GetPercentage() const { return this->percentage; }
	void SetIsExport(bool isexport) { this->isExport = isexport; }
	char* GetCurrentFormat() { return this->currentFormat; }
	char** GetFormats() { return this->formats; }
	void setFormat(char* format) { this->currentFormat = format; }

	void updatePercentage(int imageWidth, int imageHeight);

	void reset();

	void writeArray(uint32_t* imageData, int imageWidth, int imageHeight);

	void ExportImage(uint32_t* imageData, int imageWidth, int imageHeight);
};

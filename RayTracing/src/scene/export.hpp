#pragma once

#include <glm/glm.hpp>
#include <string>

class Export {
private:

	char* formats[1];
	char* currentFormat;

public:
	Export();

	char* GetCurrentFormat() { return this->currentFormat; }
	char** GetFormats() { return this->formats; }

	void setFormat(char* format) { this->currentFormat = format; }

	void ExportImage(uint32_t* imageData, int imageWidth, int imageHeight, std::string name);
};

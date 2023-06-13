#pragma once

#include <glm/glm.hpp>
#include <string>

class Export {
private:
	bool isExport;

	char* formats[1];
	char* currentFormat;

	bool finishedExport;

public:
	Export();

	bool GetIsExport() const { return this->isExport; }
	bool GetFinishedExport() const { return this->finishedExport; }
	char* GetCurrentFormat() { return this->currentFormat; }
	char** GetFormats() { return this->formats; }

	void SetIsExport(bool isexport) { this->isExport = isexport; }
	void SetFinishedExport(bool finishedExport) { this->finishedExport = finishedExport; }
	void setFormat(char* format) { this->currentFormat = format; }

	void ExportImage(uint32_t* imageData, int imageWidth, int imageHeight, std::string name);
};

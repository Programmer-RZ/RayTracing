#include "export.h"
#include "../global.h"
#include <iostream>

Export::Export() 
{
	this->reset();

	this->formats[0] = "png";
	this->formats[1] = "jpg";

	this->currentFormat = "png";
}

Export::~Export() {
	this->data.close();
}

void Export::updatePercentage(int imageWidth, int imageHeight) {
	this->percentage = static_cast<double>(this->x_index + (this->y_index) * imageWidth) / (imageWidth * imageHeight) * 100;
	//std::cout << (this->x_index * (this->y_index + 1)) << std::endl;
}

void Export::reset() {
	this->y_index = 0;
	this->x_index = 0;
	this->finishedWrite = false;
	this->finishedExport = false;
	this->isExport = false;
	this->data = std::ofstream(IMAGEDATA_PATH);
}

void Export::writeArray(uint32_t* imageData, int imageWidth, int imageHeight) {
	std::string output;
	// every iteration, right imageWidth pixels
	// or in other words, every iteration right one row

	for (int i = 0; i < imageWidth; i++) {
		if (this->y_index == 0 && this->x_index == 0) {
			// first iteration
			output = output + "P3\n" + std::to_string(imageWidth) + ' ' + std::to_string(imageHeight) + "\n255\n";
		}

		if (this->y_index >= imageHeight) {
			// overiterated
			this->finishedWrite = true;
			this->data.close();
			break;
		}

		if (this->x_index >= imageWidth) {
			// finished one row
			this->x_index = 0;
			this->y_index++;
			break;
		}
		else {
			// all requirements satisfied
			// right the pixels data
			std::vector<uint8_t> values = Utils::ConvertToFloats(imageData[x_index + y_index * imageWidth]);
			output = output + std::to_string(values[0]) + ' ' + std::to_string(values[1]) + ' ' + std::to_string(values[2]) + "\n";
			this->x_index++;
		}
	}
	this->data << output;
}

void Export::ExportImage(uint32_t* imageData, int imageWidth, int imageHeight) {
	writeArray(imageData, imageWidth, imageHeight);

	if (finishedWrite) {
		if (this->currentFormat == "png") {
			std::system(EXPORTPNG_SCRIPT);
			finishedExport = true;
		}
		else if (this->currentFormat == "jpg") {
			std::system(EXPORTJPG_SCRIPT);
			finishedExport = true;
		}
	}
}
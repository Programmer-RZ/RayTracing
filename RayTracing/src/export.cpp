#include "export.h"

Export::Export() 
{
	this->reset();
}

void Export::updatePercentage() {
	this->percentage = static_cast<int>((this->x_index * this->y_index) / (imageWidth * imageHeight) * 100.0f);
}

void Export::reset() {
	y_index = 0;
	x_index = 0;
	finishedWrite = false;
	finishedExport = false;
	data.open("..\\Data\\image_data.txt");
}

void Export::writeArray(std::string name, uint32_t* imageData, int imageWidth, int imageHeight) {
	if (x_index == 0 && y_index == 0) {
		data << "." + name + "." << std::endl;
	}

	if (x_index >= imageWidth) {
		x_index = 0;
		y_index++;
	}
	else {
		std::vector<uint8_t> values = Utils::ConvertToFloats(imageData[x_index + y_index * imageWidth]);
		for (int value = 0; value < 3; value++) {
			data << int(values[value]) << ",";
		}
		data << std::endl;
	}

	if (y_index >= imageHeight) {
		finishedWrite = true;
		data.close();
	}
	else {
		data << "row" << std::endl;
	}
}

void Export::ExportImage(std::string name, uint32_t* imageData, int imageWidth, int imageHeight) {
	if (data.is_open()) {

		writeArray(name, imageData, imageWidth, imageHeight);
	}
	else {
		std::cerr << "Couldn't open file" << std::endl;
	}

	if (finishedWrite) {
		std::system("python ..\\Helper\\export.py");
		finishedExport = true;
	}
}
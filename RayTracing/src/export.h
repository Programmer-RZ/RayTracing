#pragma once

#include <glm/glm.hpp>
#include <fstream>
#include <string>

#include "utils.h"

namespace Export {
	static void ExportImage(std::string name, uint32_t* imageData, int imageWidth, int imageHeight) {
		std::ofstream data("..\\Data\\image_data.txt");
		if (data.is_open()) {
			data << "." + name + "." << std::endl;

			for (int y = 0; y < imageHeight; y++) {
				data << "row" << std::endl;
				for (int x = 0; x < imageWidth; x++) {

					std::vector<uint8_t> values = Utils::ConvertToFloats(imageData[x + y * imageWidth]);

					for (int value = 0; value < 3; value++) {
						data << int(values[value]) << ",";
					}
					data << std::endl;
				}
			}
		}
		else {
			std::cout << "Couldn't open file" << std::endl;
		}
		data.close();

		std::system("python ..\\Helper\\export.py");
	}
}

#include "export.h"
#include "../global.h"
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb/stb_image_write.h"

Export::Export() 
	: isExport(false), finishedExport(false)
{
	this->formats[0] = "png";

	this->currentFormat = "png";
}

void Export::ExportImage(uint32_t* imageData, int imageWidth, int imageHeight, std::string name) {

	if (this->currentFormat == "png") {
		std::string path = IMAGES_PATH;
		path += name + "." + "png";

		// flip vertically so image is not flipped
		stbi_flip_vertically_on_write(true);
		stbi_write_png(path.c_str(), imageWidth, imageHeight, CHANNEL_NUMBER, imageData, imageWidth*CHANNEL_NUMBER);
	}
}
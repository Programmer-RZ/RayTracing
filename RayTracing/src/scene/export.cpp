#include "export.hpp"

#define CHANNEL_NUMBER 4

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb/stb_image_write.h"

void Export::ExportImage(uint32_t* imageData, int imageWidth, int imageHeight) {
	nfdchar_t* savepath = nullptr;
	nfdresult_t result = NFD_SaveDialog("png", NULL, &savepath);

	if (result == NFD_CANCEL || result == NFD_ERROR) {
		return;
	}

	// flip vertically so image is not flipped
	stbi_flip_vertically_on_write(true);
	stbi_write_png(savepath, imageWidth, imageHeight, CHANNEL_NUMBER, imageData, imageWidth * CHANNEL_NUMBER);
}
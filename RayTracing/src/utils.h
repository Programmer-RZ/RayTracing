#pragma once

#include "glm/glm.hpp"

#include <vector>

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color) {
		uint8_t red = uint8_t(color.r * 255.0f);
		uint8_t green = uint8_t(color.g * 255.0f);
		uint8_t blue = uint8_t(color.b * 255.0f);
		uint8_t alpha = uint8_t(color.a * 255.0f);

		uint32_t result = (alpha << 24) | (blue << 16) | (green << 8) | red;

		return result;
	}

	static std::vector<uint8_t> ConvertToFloats(const uint32_t& color) {
		uint8_t r = color & 0xff; // Extract red channel
		uint8_t g = (color >> 8) & 0xff; // Extract green channel
		uint8_t b = (color >> 16) & 0xff; // Extract blue channel
		uint8_t a = (color >> 24) & 0xff; // Extract alpha channel
		std::vector<uint8_t> rgba = { r, g, b, a };

		return rgba;
	}
}

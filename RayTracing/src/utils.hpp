#pragma once

#include "glm/glm.hpp"

#include <vector>
#include <random>
#include <string>

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
	
	static uint32_t PCG_hash(uint32_t input) {
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}
	
	static float RandomFloat(uint32_t& seed) {
		seed = PCG_hash(seed);
		return (float)seed / (float)std::numeric_limits<uint32_t>::max();
	}
	
	static glm::vec3 RandomUnitSphere(uint32_t& seed) {
		return glm::normalize(glm::vec3(RandomFloat(seed)*2.0f-1.0f, RandomFloat(seed)*2.0f-1.0f, RandomFloat(seed)*2.0f-1.0f));
	}
}

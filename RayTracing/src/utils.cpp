#include "utils.hpp"

uint32_t Utils::ConvertToRGBA(const glm::vec4& color) {
	uint8_t red = uint8_t(color.r * 255.0f);
	uint8_t green = uint8_t(color.g * 255.0f);
	uint8_t blue = uint8_t(color.b * 255.0f);
	uint8_t alpha = uint8_t(color.a * 255.0f);
	
	// alpha channel: bits 31 to 24
	// blue channel: bits 23 to 16
	// green channel: bits 15 to 8
	// red channel: bits 8 to 0
	// combine everything
	uint32_t result = (alpha << 24) | (blue << 16) | (green << 8) | red;

	return result;
}

std::vector<uint8_t> Utils::ConvertToFloats(const uint32_t& color) {
	// Right shift each color to the least significant 8 bits
	// Use & 0xff to extract the 8 bits
	// 0xff is 00000000000000000000000011111111 in binary
	// Example:
	//
	// 00011001000100111100010010010101
	// &
	// 00000000000000000000000011111111
	// = 10010101
	
	uint8_t r = color & 0xff; // Extract red channel
	uint8_t g = (color >> 8) & 0xff; // Extract green channel
	uint8_t b = (color >> 16) & 0xff; // Extract blue channel
	uint8_t a = (color >> 24) & 0xff; // Extract alpha channel
	std::vector<uint8_t> rgba = { r, g, b, a };

	return rgba;
}

uint32_t Utils::PCG_hash(uint32_t input) {
	uint32_t state = input * 747796405u + 2891336453u;
	uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

float Utils::RandomFloat(uint32_t& seed) {
	seed = Utils::PCG_hash(seed);
	return (float)seed / (float)std::numeric_limits<uint32_t>::max();
}

glm::vec3 Utils::RandomUnitSphere(uint32_t& seed) {
	return glm::normalize(glm::vec3(Utils::RandomFloat(seed) * 2.0f - 1.0f, Utils::RandomFloat(seed) * 2.0f - 1.0f, Utils::RandomFloat(seed) * 2.0f - 1.0f));
}
#pragma once

#include "glm/glm.hpp"

#include <vector>
#include <random>
#include <string>

#include <iostream>

namespace Utils {
	uint32_t ConvertToRGBA(const glm::vec4& color);

	std::vector<uint8_t> ConvertToFloats(const uint32_t& color);
	
	uint32_t PCG_hash(uint32_t input);
	
	float RandomFloat(uint32_t& seed);
	
	glm::vec3 RandomUnitSphere(uint32_t& seed);
}

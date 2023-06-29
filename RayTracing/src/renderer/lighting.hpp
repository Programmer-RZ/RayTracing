#include "hittable.hpp"

#include "../scene/scene.hpp"

#include <glm/glm.hpp>
#include "Walnut/Random.h"

namespace Scatter {
	static bool lambertian(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& multiplier, glm::vec3& rDir) {		
		multiplier = material->Albedo;
		
		light *= material->Albedo * multiplier;
		
		rDir = payload.WorldNormal + glm::normalize(Walnut::Random::InUnitSphere());
		
		return true;
	}

	static bool reflect(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& multiplier, glm::vec3& rDir) {
		multiplier = material->Albedo;
		
		light *= material->Albedo * multiplier;

		rDir = glm::reflect(rDir, payload.WorldNormal + material->roughness * Walnut::Random::InUnitSphere());
		
		return (glm::dot(rDir, payload.WorldNormal) > 0);
	}
	
	static bool diffuse_light(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& multiplier, glm::vec3& rDir) {
		multiplier = material->Albedo;

		light *= material->GetEmission();
		
		return false;
	}
}
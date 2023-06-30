#include "hittable.hpp"

#include "../scene/scene.hpp"

#include <glm/glm.hpp>
#include "Walnut/Random.h"

namespace Scatter {
	static bool lambertian(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& attenuation, glm::vec3& rDir) {		
		attenuation = material->Albedo;
		
		light *= material->Albedo * attenuation;
		
		rDir = payload.WorldNormal + glm::normalize(Walnut::Random::InUnitSphere());
		
		if (fabs(rDir.x) < 1e-8 && fabs(rDir.y) < 1e-8 && fabs(rDir.z) < 1e-8) {
			rDir = payload.WorldNormal;
		}
		
		return true;
	}

	static bool reflect(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& attenuation, glm::vec3& rDir) {
		attenuation = material->Albedo;
		
		light *= material->Albedo * attenuation;

		rDir = glm::reflect(rDir, payload.WorldNormal + material->roughness * Walnut::Random::InUnitSphere());
		
		return (glm::dot(rDir, payload.WorldNormal) > 0);
	}
	
	static bool diffuse_light(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& attenuation, glm::vec3& rDir) {
		attenuation = material->Albedo;

		light *= material->GetEmission();
		
		return false;
	}
}
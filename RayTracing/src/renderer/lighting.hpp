#include "hittable.hpp"

#include "../scene/scene.hpp"

#include <glm/glm.hpp>
#include "Walnut/Random.h"

namespace Lighting {
	static void lambertian(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& multiplier, glm::vec3& rOrg, glm::vec3& rDir, bool& no_scatter) {
		light += material->Albedo * multiplier;
		multiplier *= material->Albedo;

		rOrg = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		rDir = payload.WorldNormal + glm::normalize(Walnut::Random::InUnitSphere());
		
		no_scatter = false;
	}

	static void reflect(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& multiplier, glm::vec3& rOrg, glm::vec3& rDir, bool& no_scatter) {
		light += material->Albedo * multiplier;
		multiplier *= material->Albedo;

		rOrg = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		rDir = glm::reflect(rDir, payload.WorldNormal + material->roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
		
		no_scatter = false;
	}
	
	static void diffuse_light(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& multiplier, glm::vec3& rOrg, glm::vec3& rDir, bool& no_scatter) {
		light += material->GetEmission();
		multiplier *= material->Albedo;

		rOrg = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		rDir = glm::reflect(rDir, payload.WorldNormal + material->roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
	
		no_scatter = true;
	}
}
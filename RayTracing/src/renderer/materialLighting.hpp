#include "hittable.hpp"

#include "../scene/scene.hpp"

#include <glm/glm.hpp>
#include "Walnut/Random.h"

namespace Lighting {
	static void diffuse(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& multiplier, glm::vec3& rOrg, glm::vec3& rDir) {
		multiplier *= material->Albedo;
		light += material->GetEmission();

		rOrg = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		rDir = glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere());
	}

	static void scatter(HitPayload payload, const Material* material, glm::vec3& light, glm::vec3& multiplier, glm::vec3& rOrg, glm::vec3& rDir) {
		light += material->Albedo * multiplier;
		multiplier *= 0.3f;

		rOrg = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		rDir = glm::reflect(rDir, payload.WorldNormal + material->roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
	}
}
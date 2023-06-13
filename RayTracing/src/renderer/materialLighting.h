#include "hittable.h"

#include <glm/glm.hpp>
#include "Walnut/Random.h"

namespace Lighting {
	static void diffuse(HitPayload payload, glm::vec3 emission, glm::vec3 albedo, glm::vec3& light, glm::vec3& multiplier, glm::vec3& rOrg, glm::vec3& rDir) {
		multiplier *= albedo;
		light += emission;

		rOrg = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		rDir = glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere());
	}
}
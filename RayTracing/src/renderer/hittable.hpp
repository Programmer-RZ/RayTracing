#pragma once

#include "glm/glm.hpp"
#include "../scene/scene.hpp"
#include "ray.hpp"

struct HitPayload {
	float HitDist = 0.0f;
	float u;
	float v;

	bool miss = true;
	bool front_face;

	glm::vec3 WorldPosition = { 0.0f, 0.0f, 0.0f };
	glm::vec3 WorldNormal = { 0.0f, 0.0f, 0.0f };

	const Material* materialPtr;
};


namespace SphereIntersection {
	void TraceRay(const Ray& ray, const Scene* ActiveScene, HitPayload& payload);
};

namespace BoxIntersection {
	void TraceRay(const Ray& ray, const Scene* ActiveScene, HitPayload& payload);
	void xyrect_TraceRay(const Ray& ray, const Scene* ActiveScene, xy_rect& xyrect, HitPayload& payload);
	void xzrect_TraceRay(const Ray& ray, const Scene* ActiveScene, xz_rect& xzrect, HitPayload& payload);
	void yzrect_TraceRay(const Ray& ray, const Scene* ActiveScene, yz_rect& xzrect, HitPayload& payload);
}




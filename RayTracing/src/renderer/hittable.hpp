#pragma once

#include "glm/glm.hpp"
#include "../scene/scene.hpp"
#include "ray.hpp"

struct HitPayload {
	float HitDist;

	glm::vec3 WorldPosition;
	glm::vec3 WorldNormal;

	int ObjectIndex;
	const Material* materialPtr;

	glm::vec3 pos;
};

struct Hittable {

	Hittable() = default;

	virtual void TraceRay(const Ray& ray, const Scene* ActiveScene, int& closestObject, float& hitDist, Hittable*& object);
	virtual void ClosestHit(const Ray& ray, const Scene* scene, HitPayload& payload);
};



struct SphereIntersection : public Hittable {
	virtual void TraceRay(const Ray& ray, const Scene* ActiveScene, int& closestObject, float& hitDist, Hittable*& object) override;
	virtual void ClosestHit(const Ray& ray, const Scene* scene, HitPayload& payload) override;
};




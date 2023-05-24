#pragma once

#include "glm/glm.hpp"
#include "scene.h"
#include "ray.h"

class Hittable {

public:
	Hittable() = default;

	virtual void TraceRay(const Ray& ray, const Scene* ActiveScene, int& closestObject, float& hitDist, std::string& objectName) {}
};



class SphereIntersection : public Hittable {
public:
	virtual void TraceRay(const Ray& ray, const Scene* ActiveScene, int& closestObject, float& hitDist, std::string& objectName) override;
};


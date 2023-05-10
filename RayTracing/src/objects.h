#pragma once

#include "glm/glm.hpp"
#include "scene.h"
#include "ray.h"

class Object {

public:
	Object() = default;

	virtual void TraceRay(const Ray& ray, const Scene* ActiveScene, int& closestObject, float& hitDist, std::string& objectName) {}
};



class SphereIntersection : public Object {
public:
	virtual void TraceRay(const Ray& ray, const Scene* ActiveScene, int& closestObject, float& hitDist, std::string& objectName) override {
		// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0

		// a = ray origin
		// b = ray direction
		// r = radius of sphere
		// t = hit distance

		for (int i = 0; i < ActiveScene->spheres.size(); i++) {

			const Sphere& sphere = ActiveScene->spheres[i];
			glm::vec3 origin = ray.Origin - sphere.pos;

			float a = glm::dot(ray.Direction, ray.Direction);
			float b = 2.0f * glm::dot(origin, ray.Direction);
			float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

			// Quadratic formula discriminent:
			// b^2 - 4ac

			float discriminent = b * b - 4.0f * a * c;

			if (discriminent < 0) {
				continue;
			}

			// (-b +- sqrt(discriminent)) / 2a
			// float t0 = (-b + glm::sqrt(discriminent)) / (2.0f * a);
			float closestT = (-b - glm::sqrt(discriminent)) / (2.0f * a);

			if (closestT > 0.0f && closestT < hitDist) {
				hitDist = closestT;
				closestObject = i;
				objectName = "sphere";
			}
		}
	}
};

#include "hittable.hpp"
#include <iostream>


void SphereIntersection::TraceRay(const Ray& ray, const Scene* ActiveScene, HitPayload& payload) {
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

		if (closestT < 0.0f || closestT > payload.HitDist) {
			continue;
		}

		payload.HitDist = closestT;
		payload.miss = false;

		payload.WorldPosition = origin + ray.Direction * payload.HitDist;
		
		payload.front_face = glm::dot(ray.Direction, glm::normalize(payload.WorldPosition)) < 0;
		payload.WorldNormal = payload.front_face ? glm::normalize(payload.WorldPosition) : -glm::normalize(payload.WorldPosition);

		payload.WorldPosition += sphere.pos;
		payload.materialPtr = &(ActiveScene->materials[sphere.material_index]);
	}
}

void BoxIntersection::TraceRay(const Ray& ray, const Scene* ActiveScene, HitPayload& payload)
{
	for (int i = 0; i < ActiveScene->boxes.size(); i++) {
		const Box& box = ActiveScene->boxes[i];

		xy_rect side1 = box.side1;
		xy_rect side2 = box.side2;

		xz_rect side3 = box.side3;
		xz_rect side4 = box.side4;

		yz_rect side5 = box.side5;
		yz_rect side6 = box.side6;

		BoxIntersection::xyrect_TraceRay(ray, ActiveScene, side1, payload);
		BoxIntersection::xyrect_TraceRay(ray, ActiveScene, side2, payload);

		BoxIntersection::xzrect_TraceRay(ray, ActiveScene, side3, payload);
		BoxIntersection::xzrect_TraceRay(ray, ActiveScene, side4, payload);

		BoxIntersection::yzrect_TraceRay(ray, ActiveScene, side5, payload);
		BoxIntersection::yzrect_TraceRay(ray, ActiveScene, side6, payload);
	}
}

void BoxIntersection::xyrect_TraceRay(const Ray& ray, const Scene* ActiveScene, xy_rect& xyrect, HitPayload& payload)
{
	float closestT = (xyrect.k - ray.Origin.z) / ray.Direction.z;

	if (closestT < 0.0f || closestT > payload.HitDist) {
		return;
	}

	float x = ray.Origin.x + closestT * ray.Direction.x;
	float y = ray.Origin.y + closestT * ray.Direction.y;

	if (x < xyrect.x0 || x > xyrect.x1 || y < xyrect.y0 || y > xyrect.y1) {
		return;
	}

	payload.HitDist = closestT;
	payload.miss = false;

	payload.WorldPosition = ray.Origin + ray.Direction * payload.HitDist;

	payload.front_face = glm::dot(ray.Direction, glm::vec3{ 0, 0, 1 }) < 0;
	payload.WorldNormal = payload.front_face ? glm::vec3{ 0, 0, 1 } : -glm::vec3{ 0, 0, 1 };

	payload.materialPtr = &(ActiveScene->materials[xyrect.material_index]);
}

void BoxIntersection::xzrect_TraceRay(const Ray& ray, const Scene* ActiveScene, xz_rect& xzrect, HitPayload& payload)
{
	float closestT = (xzrect.k - ray.Origin.y) / ray.Direction.y;

	if (closestT < 0.0f || closestT > payload.HitDist) {
		return;
	}

	float x = ray.Origin.x + closestT * ray.Direction.x;
	float z = ray.Origin.z + closestT * ray.Direction.z;

	if (x < xzrect.x0 || x > xzrect.x1 || z < xzrect.z0 || z > xzrect.z1) {
		return;
	}

	payload.HitDist = closestT;
	payload.miss = false;

	payload.WorldPosition = ray.Origin + ray.Direction * payload.HitDist;

	payload.front_face = glm::dot(ray.Direction, glm::vec3{ 0, 1, 0 }) < 0;
	payload.WorldNormal = payload.front_face ? glm::vec3{ 0, 1, 0 } : -glm::vec3{ 0, 1, 0 };

	payload.materialPtr = &(ActiveScene->materials[xzrect.material_index]);
}

void BoxIntersection::yzrect_TraceRay(const Ray& ray, const Scene* ActiveScene, yz_rect& xzrect, HitPayload& payload)
{
	float closestT = (xzrect.k - ray.Origin.x) / ray.Direction.x;

	if (closestT < 0.0f || closestT > payload.HitDist) {
		return;
	}

	float y = ray.Origin.y + closestT * ray.Direction.y;
	float z = ray.Origin.z + closestT * ray.Direction.z;

	if (y < xzrect.y0 || y > xzrect.y1 || z < xzrect.z0 || z > xzrect.z1) {
		return;
	}

	payload.HitDist = closestT;
	payload.miss = false;

	payload.WorldPosition = ray.Origin + ray.Direction * payload.HitDist;

	payload.front_face = glm::dot(ray.Direction, glm::vec3{ 1, 0, 0 }) < 0;
	payload.WorldNormal = payload.front_face ? glm::vec3{ 1, 0, 0 } : -glm::vec3{ 1, 0, 0 };

	payload.materialPtr = &(ActiveScene->materials[xzrect.material_index]);
}

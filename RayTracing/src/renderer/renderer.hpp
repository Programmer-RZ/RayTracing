#pragma once

#include "Walnut/Image.h"

#include "camera.hpp"
#include "ray.hpp"
#include "../scene/scene.hpp"

#include <memory>
#include "glm/glm.hpp"
#include "hittable.hpp"

class Renderer {	

public:
	Renderer() = default;

	void resetFrameIndex() { this->frameIndex = 1; }

	void render(const Scene& scene, const Camera& camera, glm::vec3& skycolor);

	bool on_resize(uint32_t width, uint32_t height);

	void realisticRender();

	// getters
	std::shared_ptr<Walnut::Image> get_final_image() const { return this->m_FinalImage; }
	uint32_t* GetImageData() const { return this->imageData; }
	int GetImageWidth() const { return this->m_FinalImage->GetWidth(); }
	int GetImageHeight() const { return this->m_FinalImage->GetHeight(); }
	bool GetRealisticRendering() const { return this->realisticRendering; }
	bool GetFinishedRealistic() const { return this->finishedRealistic; }
	bool GetCameraMoved() const { return this->cameraMoved; }
	int& GetBounces() { return this->bounces; }

	// setters
	void SetFinishRealistic() { this->realisticRendering = false; this->finishedRealistic = false; this->coherence = 7; this->bounces = 2; }
	void SetCameraMoved(bool cameraMoved) { this->cameraMoved = cameraMoved; }

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* imageData = nullptr;
	glm::vec4* AccumulationData = nullptr;

	const Scene* ActiveScene = nullptr;
	const Camera* ActiveCamera = nullptr;

	// intersection formulas
	SphereIntersection sphere_intersection = SphereIntersection();

	// rendering
	int frameIndex = 1;
	int maxFrameIndex = 5;

	// scene
	bool cameraMoved = false;

	// realistic rendering
	int bounces = 2;
	int coherence = 7;
	bool realisticRendering = false;
	bool finishedRealistic = false;

private:

	glm::vec4 PerPixel(uint32_t x, uint32_t y, glm::vec3& skycolor);

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDist, int objectIndex, Hittable* object);
	HitPayload Miss(const Ray& ray);
};

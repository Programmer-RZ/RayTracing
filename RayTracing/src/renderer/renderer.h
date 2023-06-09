#pragma once

#include "Walnut/Image.h"

#include "camera.h"
#include "ray.h"
#include "../scene/scene.h"
#include "hittable.h"

#include <memory>
#include "glm/glm.hpp"

class Renderer {	

public:
	Renderer() = default;

	void resetFrameIndex() { this->frameIndex = 1; }

	void render(const Scene& scene, const Camera& camera, glm::vec3& lightDir, glm::vec3& skycolor, glm::vec3& lightPos);

	bool on_resize(uint32_t width, uint32_t height);

	void realisticRender();

	// getters
	std::shared_ptr<Walnut::Image> get_final_image() const { return this->m_FinalImage; }
	uint32_t* GetImageData() const { return this->imageData; }
	int GetImageWidth() const { return this->m_FinalImage->GetWidth(); }
	int GetImageHeight() const { return this->m_FinalImage->GetHeight(); }
	bool GetRealisticRendering() const { return this->realisticRendering; }
	bool GetFinishedRealistic() const { return this->finishedRealistic; }
	bool GetSceneMoved() const { return this->sceneMoved; }
	bool GetCameraMoved() const { return this->cameraMoved; }
	int& GetBounces() { return this->bounces; }
	float& GetBrightness() { return this->brightness; }

	// setters
	void SetFinishRealisticAndExport() { this->realisticRendering = false; this->finishedRealistic = false; this->coherence = 7; this->bounces = 2; }
	void SetSceneMoved(bool sceneMoved) { this->sceneMoved = sceneMoved; }
	void SetCameraMoved(bool cameraMoved) { this->cameraMoved = cameraMoved; }

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* imageData = nullptr;
	glm::vec4* AccumulationData = nullptr;

	const Scene* ActiveScene = nullptr;
	const Camera* ActiveCamera = nullptr;

	// intersection formulas
	SphereIntersection sphere_intersection = SphereIntersection();

	// accumulation
	int frameIndex = 1;

	// scene
	bool sceneMoved = true;
	bool cameraMoved = false;

	// realistic rendering
	int bounces = 2;
	int coherence = 5;
	float brightness = 0.3f;
	bool realisticRendering = false;
	bool finishedRealistic = false;
	int realisticCount = 0;
	int maxRealisticCount = 50;

private:

	glm::vec4 PerPixel(uint32_t x, uint32_t y, glm::vec3& lightDir, glm::vec3& skycolor, glm::vec3& lightPos);

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDist, int objectIndex, Hittable* object);
	HitPayload Miss(const Ray& ray);

private:
	void resetArray();
};

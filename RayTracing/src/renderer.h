#pragma once

#include "Walnut/Image.h"

#include "camera.h"
#include "ray.h"
#include "scene.h"
#include "hittable.h"

#include <memory>
#include "glm/glm.hpp"

class Renderer {

private:
	struct HitPayload {
		float HitDist;

		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

public:
	struct Settings {
		bool Accumulate = true;
	};

public:
	Renderer() = default;

	void resetFrameIndex() { this->frameIndex = 1; }

	void render(const Scene& scene, const Camera& camera);

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
	Settings& GetSettings() { return this->settings; }
	glm::vec3& GetLightDir() { return this->lightDir; }
	glm::vec3& GetSkycolor() { return this->skycolor; }
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

	Settings settings;

	glm::vec3 lightDir = glm::vec3(-0.546f, -0.722f, -0.425f);
	glm::vec3 skycolor = glm::vec3(0.0f, 0.0f, 0.0f);

	int frameIndex = 1;
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

	glm::vec4 PerPixel(uint32_t x, uint32_t y);

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDist, int objectIndex, std::string name);
	HitPayload Miss(const Ray& ray);

private:
	void resetArray();
};

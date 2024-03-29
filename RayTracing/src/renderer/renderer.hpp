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

	void render(const Scene& scene, const Camera& camera, const glm::vec3& skycolor);

	bool on_resize(const uint32_t width, const uint32_t height);

	void SetupFinalImage();

	// getters
	std::shared_ptr<Walnut::Image> get_final_image() const { return this->m_FinalImage; }
	uint32_t* GetImageData() const { return this->imageData; }
	int GetImageWidth() const { return this->m_FinalImage->GetWidth(); }
	int GetImageHeight() const { return this->m_FinalImage->GetHeight(); }
	bool IsRenderingFinalImage() const { return this->renderingFinalImage; }
	bool IsFinishedFinalImage() const { return this->finishedFinalImage; }
	bool IsCameraMoved() const { return this->cameraMoved; }
	
	int& GetBounces() { return this->bounces; }
	
	int* GetFinalImageBounces() { return &(this->finalImage_bounces); }
	int* GetFinalImageIterations() { return &(this->finalImage_maxFrameIndex); }
	int* GetFinalImageCoherence() { return &(this->finalImage_coherence); }

	// setters
	void SetFinishedFinalImage() { this->renderingFinalImage = false; this->resetFrameIndex(); this->finishedFinalImage = false; this->coherence = 3; this->bounces = 7; this->maxFrameIndex = 10; }
	void SetCameraMoved(bool cameraMoved) { this->cameraMoved = cameraMoved; }

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* imageData = nullptr;
	glm::vec4* AccumulationData = nullptr;

	const Scene* ActiveScene = nullptr;
	const Camera* ActiveCamera = nullptr;

	// rendering
	int frameIndex = 1;
	int maxFrameIndex = 10;
	int bounces = 7;
	int coherence = 3;
	
	// scene
	bool cameraMoved = false;

	// final image
	bool renderingFinalImage = false;
	bool finishedFinalImage = false;
	
	int finalImage_maxFrameIndex = 60;
	int finalImage_bounces = 10;
	int finalImage_coherence = 1;

private:

	glm::vec4 PerPixel(const uint32_t x, const uint32_t y, Ray& ray);

	HitPayload TraceRay(const Ray& ray);
};

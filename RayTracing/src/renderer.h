#pragma once

#include "Walnut/Image.h"

#include "camera.h"
#include "ray.h"
#include "scene.h"

#include <memory>
#include "glm/glm.hpp"

class Renderer {
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* imageData = nullptr;

	const Scene* ActiveScene = nullptr;
	const Camera* ActiveCamera = nullptr;

private:
	struct HitPayload {
		float HitDist;

		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y);

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDist, int objectIndex);
	HitPayload Miss(const Ray& ray);

public:
	Renderer() = default;

	uint32_t* GetImageData() const { return this->imageData; }
	int GetImageWidth() const { return this->m_FinalImage->GetWidth(); }
	int GetImageHeight() const { return this->m_FinalImage->GetHeight(); }

	void render(const Scene& scene, const Camera& camera);

	void on_resize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> get_final_image() const { return this->m_FinalImage; }
};

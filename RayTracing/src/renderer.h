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

private:
	glm::vec4 TraceRay(const Scene& scene, const Ray& ray);

public:
	Renderer() = default;

	uint32_t* GetImageData() const { return this->imageData; }
	int GetImageWidth() const { return this->m_FinalImage->GetWidth(); }
	int GetImageHeight() const { return this->m_FinalImage->GetHeight(); }

	void render(const Scene& scene, const Camera& camera);

	void on_resize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> get_final_image() const { return this->m_FinalImage; }
};

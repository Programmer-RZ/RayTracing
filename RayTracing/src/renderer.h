#pragma once

#include "Walnut/Image.h"

#include "camera.h"
#include "ray.h"

#include <memory>
#include "glm/glm.hpp"

class Renderer {
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* imageData = nullptr;

private:
	glm::vec4 TraceRay(const Ray& ray);

public:
	Renderer() = default;

	void render(const Camera& camera);

	void on_resize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> get_final_image() const { return this->m_FinalImage; }
};

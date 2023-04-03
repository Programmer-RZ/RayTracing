#include "renderer.h"

#include "Walnut/Random.h"

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color) {
		uint8_t red = uint8_t(color.r * 255.0f);
		uint8_t green = uint8_t(color.g * 255.0f);
		uint8_t blue = uint8_t(color.b * 255.0f);
		uint8_t alpha = uint8_t(color.a * 255.0f);

		uint32_t result = (alpha << 24) | (blue << 16) | (green << 8) | red;

		return result;
	}
}

void Renderer::on_resize(uint32_t width, uint32_t height) {

	if (this->m_FinalImage) {

		// no need to resize
		if (this->m_FinalImage->GetWidth() == width && this->m_FinalImage->GetHeight() == height) {
			return;
		}

		this->m_FinalImage->Resize(width, height);
	}
	else {
		this->m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] this->imageData;
	this->imageData = new uint32_t[width * height];
}


void Renderer::render(const Camera& camera) {

	Ray ray;
	ray.Origin = camera.GetPosition();

	for (uint32_t y = 0; y < this->m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < this->m_FinalImage->GetWidth(); x++) {

			ray.Direction = camera.GetRayDirections()[x + y * this->m_FinalImage->GetWidth()];

			glm::vec4 color = this->TraceRay(ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			this->imageData[x + y * this->m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	this->m_FinalImage->SetData(this->imageData);
}

glm::vec4 Renderer::TraceRay(const Ray& ray)
{

	float radius = 0.5f;

	// rayDir = glm::normalize(rayDir);

	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0

	// a = ray origin
	// b = ray direction
	// r = radius of sphere
	// t = hit distance

	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
	float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

	// Quadratic formula discriminent:
	// b^2 - 4ac

	float discriminent = b * b - 4.0f * a * c;

	if (discriminent < 0) {
		return glm::vec4(0, 0, 0, 1);
	}

	// (-b +- sqrt(discriminent)) / 2a
	// float t0 = (-b + glm::sqrt(discriminent)) / (2.0f * a);
	float closestT = (-b - glm::sqrt(discriminent)) / (2.0f * a);

	glm::vec3 hitPoint = ray.Origin + ray.Direction * closestT;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	float d = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

	glm::vec3 sphereColor(0.25, 0.25, 0.25);
	sphereColor *= d;
	return glm::vec4(sphereColor, 1.0f);
}

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


void Renderer::render(const Scene& scene, const Camera& camera) {

	Ray ray;
	ray.Origin = camera.GetPosition();

	for (uint32_t y = 0; y < this->m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < this->m_FinalImage->GetWidth(); x++) {

			ray.Direction = camera.GetRayDirections()[x + y * this->m_FinalImage->GetWidth()];

			glm::vec4 color = this->TraceRay(scene, ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			this->imageData[x + y * this->m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	this->m_FinalImage->SetData(this->imageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{

	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0

	// a = ray origin
	// b = ray direction
	// r = radius of sphere
	// t = hit distance

	if (scene.spheres.size() == 0) {
		return glm::vec4(0, 0, 0, 1);
	}


	const Sphere* closestSphere = nullptr;
	float hitDist = std::numeric_limits<float>::max();

	for (const Sphere& sphere : scene.spheres) {

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

		if (closestT < hitDist) {
			hitDist = closestT;
			closestSphere = &sphere;
		}
	}
	
	if (closestSphere == nullptr) {
		return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	glm::vec3 origin = ray.Origin - closestSphere->pos;
	glm::vec3 hitPoint = origin + ray.Direction * hitDist;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	float d = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

	glm::vec3 sphereColor = closestSphere->Albedo;
	sphereColor *= d;
	return glm::vec4(sphereColor, 1.0f);
}

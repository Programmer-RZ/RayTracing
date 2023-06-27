#include "renderer.hpp"

#include <iostream>
#include <cmath>

#include "../utils.hpp"

#include "lighting.hpp"

void Renderer::realisticRender() {
	this->resetFrameIndex();

	this->coherence = 1;
	this->bounces = 5;
	this->realisticRendering = true;
	this->maxFrameIndex = 50;
}

bool Renderer::on_resize(uint32_t width, uint32_t height) {

	if (this->m_FinalImage) {

		// no need to resize
		if (this->m_FinalImage->GetWidth() == width && this->m_FinalImage->GetHeight() == height) {
			return false;
		}

		this->m_FinalImage->Resize(width, height);
	}
	else {
		this->m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] this->imageData;
	this->imageData = new uint32_t[width * height];

	delete[] this->AccumulationData;
	this->AccumulationData = new glm::vec4[width * height];

	return true;
}

void Renderer::render(const Scene& scene, const Camera& camera, glm::vec3& skycolor) {
	if (this->frameIndex > this->maxFrameIndex) {
		// no need to calculate the image again
		if (this->realisticRendering) {
			this->finishedRealistic = true;
		}
		return;
	}

	this->ActiveScene = &scene;
	this->ActiveCamera = &camera;

	// reset frame index if realistic rendering
	if (this->frameIndex == 1) {
		memset(this->AccumulationData, 0, this->m_FinalImage->GetWidth() * this->m_FinalImage->GetHeight() * sizeof(glm::vec4));
	}

	for (uint32_t y = 0; y < this->m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = this->coherence - 1; x < this->m_FinalImage->GetWidth(); x+=this->coherence) {

			glm::vec4 color = this->PerPixel(x, y, skycolor);
			uint32_t RGBA;

			for (int c = 0; c < this->coherence; c++) {
				this->AccumulationData[(x - c) + y * this->m_FinalImage->GetWidth()] += color;
			}

			glm::vec4 accumulatedColor = this->AccumulationData[x + y * this->m_FinalImage->GetWidth()];
			accumulatedColor /= this->frameIndex;

			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
			RGBA = Utils::ConvertToRGBA(accumulatedColor);

			for (int c = 0; c < this->coherence; c++) {
				this->imageData[(x - c) + y * this->m_FinalImage->GetWidth()] = RGBA;
			}
		}
	}

	this->m_FinalImage->SetData(this->imageData);

	this->frameIndex++;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y, glm::vec3& skycolor)
{
	Ray ray;
	ray.Origin = ActiveCamera->GetPosition();
	ray.Direction = ActiveCamera->GetRayDirections()[x + y * this->m_FinalImage->GetWidth()];
	
	glm::vec3 light(0.0f, 0.0f, 0.0f);
	glm::vec3 multiplier(1.0f);

	for (int i = 0; i < this->bounces; i++) {
		HitPayload payload = this->TraceRay(ray);

		//hitDist = std::round(hitDist * 1000) / 1000;
		//std::cout << hitDist << std::endl;
		
		if (payload.miss) {
			light += skycolor * multiplier;
			break;
		}

		const Material* material = payload.materialPtr;

		if (material->lighting == "diffuse") {
			Lighting::diffuse(payload, material, light, multiplier, ray.Origin, ray.Direction);
		}
		else if (material->lighting == "reflect") {
			Lighting::reflect(payload, material, light, multiplier, ray.Origin, ray.Direction);
		}
	}

	return glm::vec4(light, 1.0f);
}


HitPayload Renderer::TraceRay(const Ray& ray)
{
	HitPayload payload;
	payload.HitDist = std::numeric_limits<float>::max();

	SphereIntersection::TraceRay(ray, this->ActiveScene, payload);
	BoxIntersection::TraceRay(ray, this->ActiveScene, payload);

	return payload;
}

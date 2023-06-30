#include "renderer.hpp"

#include <iostream>
#include <cmath>

#include "../utils.hpp"

#include "lighting.hpp"

void Renderer::SetupFinalImage() {
	this->resetFrameIndex();

	this->coherence = 1;
	this->bounces = 3;
	this->renderingFinalImage = true;
	this->maxFrameIndex = 20;
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
		if (this->renderingFinalImage) {
			this->finishedFinalImage = true;
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
			
			Ray ray;
			ray.Origin = this->ActiveCamera->GetPosition();
			ray.Direction = this->ActiveCamera->CalculateRayDirection(x, y);
			
			glm::vec4 color = this->PerPixel(x, y, ray);
			
			// gamma correction
			color.r = sqrt(color.r);
			color.g = sqrt(color.g);
			color.b = sqrt(color.b);
		
			
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

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y, Ray& ray)
{
	
	glm::vec3 light(1.0f, 1.0f, 1.0f);
	glm::vec3 attenuation(1.0f);
	uint32_t seed = x + y * this->m_FinalImage->GetWidth();
	seed *= this->frameIndex;
	
	bool scatter = true;

	for (int i = 0; i < this->bounces; i++) {
		seed += i;
		
		
		HitPayload payload = this->TraceRay(ray);
		
		if (payload.miss) {
			light *= attenuation * this->ActiveScene->skycolor;
			
			break;
		}

		const Material* material = payload.materialPtr;
		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		if (material->lighting == "lambertian") {
			scatter = Scatter::lambertian(payload, material, light, attenuation, ray.Direction, seed);
		}
		else if (material->lighting == "reflect") {
			scatter = Scatter::reflect(payload, material, light, attenuation, ray.Direction, seed);
		}
		else if (material->lighting == "diffuse light") {
			scatter = Scatter::diffuse_light(payload, material, light, attenuation, ray.Direction, seed);
		}
		
		if (!scatter) { break; }
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

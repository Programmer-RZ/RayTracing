#include "renderer.h"

#include "Walnut/Random.h"

#include <iostream>
#include <cmath>

#include "../utils.h"

void Renderer::realisticRender() {
	//this->resetFrameIndex();

	this->coherence = 1;
	this->bounces = 10;
	this->realisticRendering = true;
	this->maxFrameIndex = 150;

	this->frameIndex = 2;
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

		float hitDist = payload.HitDist;

		//hitDist = std::round(hitDist * 1000) / 1000;
		//std::cout << hitDist << std::endl;
		
		if (hitDist < 0.0f) {
			light += skycolor * multiplier;
			break;
		}

		const Material* material = payload.materialPtr;


		glm::vec3 albedo = material->Albedo;

		multiplier *= albedo;
		light += material->GetEmission();

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;

		//ray.Direction = glm::reflect(ray.Direction, 
		//	payload.WorldNormal + material->roughness * Walnut::Random::Vec3(-0.5f, 0.5f));

		ray.Direction = glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere());
	}

	return glm::vec4(light, 1.0f);
}

HitPayload Renderer::ClosestHit(const Ray& ray, float hitDist, int objectIndex, Hittable* object)
{
	HitPayload payload;
	payload.HitDist = hitDist;
	payload.ObjectIndex = objectIndex;

	object->ClosestHit(ray, this->ActiveScene, payload);

	return payload;
}

HitPayload Renderer::Miss(const Ray& ray)
{
	HitPayload payload;
	payload.HitDist = -1.0f;

	return payload;
}

HitPayload Renderer::TraceRay(const Ray& ray)
{
	int closestObject = -1;
	float hitDist = std::numeric_limits<float>::max();

	// default no object in scene
	Hittable noObj = Hittable();
	Hittable* object = &noObj;

	this->sphere_intersection.TraceRay(ray, this->ActiveScene, closestObject, hitDist, object);
	
	if (closestObject < 0) {
		return this->Miss(ray);
	}

	return this->ClosestHit(ray, hitDist, closestObject, object);
}

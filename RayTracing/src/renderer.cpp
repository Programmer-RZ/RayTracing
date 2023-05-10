#include "renderer.h"

#include "Walnut/Random.h"

#include <iostream>
#include <cmath>

#include "utils.h"

#include "objects.h"

void Renderer::realisticRender() {
	this->resetArray();

	this->coherence = 1;
	this->bounces = 50;
	this->realisticRendering = true;
}

void Renderer::resetArray() {
	uint32_t width = this->m_FinalImage->GetWidth();
	uint32_t height = this->m_FinalImage->GetHeight();

	delete[] this->imageData;
	this->imageData = new uint32_t[width * height];

	delete[] this->AccumulationData;
	this->AccumulationData = new glm::vec4[width * height];
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

	delete[] this->AccumulationData;
	this->AccumulationData = new glm::vec4[width * height];
}

void Renderer::render(const Scene& scene, const Camera& camera) {

	this->ActiveScene = &scene;
	this->ActiveCamera = &camera;

	if (this->realisticRendering) {
		if (this->frameIndex == 1) {
			memset(this->AccumulationData, 0, this->m_FinalImage->GetWidth() * this->m_FinalImage->GetHeight() * sizeof(glm::vec4));
		}
	}

	for (uint32_t y = 0; y < this->m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = this->coherence - 1; x < this->m_FinalImage->GetWidth(); x+=this->coherence) {

			glm::vec4 color = this->PerPixel(x, y);
			uint32_t RGBA;

			if (this->realisticRendering) {
				for (int c = 0; c < this->coherence; c++) {
					this->AccumulationData[(x - c) + y * this->m_FinalImage->GetWidth()] += color;
				}

				glm::vec4 accumulatedColor = this->AccumulationData[x + y * this->m_FinalImage->GetWidth()];
				accumulatedColor /= this->frameIndex;

				accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
				RGBA = Utils::ConvertToRGBA(accumulatedColor);
			}
			else {
				color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
				RGBA = Utils::ConvertToRGBA(color);
			}

			for (int c = 0; c < this->coherence; c++) {
				this->imageData[(x - c) + y * this->m_FinalImage->GetWidth()] = RGBA;
			}
		}
	}

	this->m_FinalImage->SetData(this->imageData);

	if (this->realisticRendering) {
		if (this->settings.Accumulate) {
			this->frameIndex++;
		}
		else {
			this->frameIndex = 1;
		}
	}

	if (this->realisticRendering) {
		this->realisticCount++;

		if (this->realisticCount >= this->maxRealisticCount) {
			this->realisticCount = 0;
			this->finishedRealistic = true;
		}
	}
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = ActiveCamera->GetPosition();
	ray.Direction = ActiveCamera->GetRayDirections()[x + y * this->m_FinalImage->GetWidth()];
	
	glm::vec3 color(0.0f, 0.0f, 0.0f);
	float multiplier = 1.0f;

	for (int i = 0; i < this->bounces; i++) {
		Renderer::HitPayload payload = this->TraceRay(ray);

		float hitDist = payload.HitDist;

		//hitDist = std::round(hitDist * 1000) / 1000;
		//std::cout << hitDist << std::endl;
		
		if (hitDist < 0.0f) {
			glm::vec3 skycolor = glm::vec3(0.6f, 0.7f, 0.9f);
			color += skycolor * multiplier;
			break;
		}

		this->lightDir = glm::normalize(this->lightDir);
		float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -this->lightDir), 0.0f); // == cos(angle)

		const Sphere& sphere = this->ActiveScene->spheres[payload.ObjectIndex];
		const Material& material = this->ActiveScene->materials[sphere.material_index];

		glm::vec3 sphereColor = material.Albedo;
		sphereColor *= lightIntensity;

		color += sphereColor * multiplier;

		multiplier *= this->brightness;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::reflect(ray.Direction, 
			payload.WorldNormal + material.roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
	}

	return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDist, int objectIndex, std::string name)
{
	Renderer::HitPayload payload;
	payload.HitDist = hitDist;
	payload.ObjectIndex = objectIndex;

	if (name == "sphere") {
		const Sphere& closestSphere = ActiveScene->spheres[objectIndex];
		glm::vec3 origin = ray.Origin - closestSphere.pos;
		payload.WorldPosition = origin + ray.Direction * hitDist;
		payload.WorldNormal = glm::normalize(payload.WorldPosition);

		payload.WorldPosition += closestSphere.pos;
	}

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDist = -1.0f;
	return payload;
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	int closestObject = -1;
	float hitDist = std::numeric_limits<float>::max();
	std::string objectName;


	SphereIntersection sphere_intersection = SphereIntersection();
	sphere_intersection.TraceRay(ray, this->ActiveScene, closestObject, hitDist, objectName);
	
	if (closestObject < 0) {
		return this->Miss(ray);
	}

	return this->ClosestHit(ray, hitDist, closestObject, objectName);
}

#include "renderer.h"

#include "Walnut/Random.h"

#include "utils.h"

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

	if (this->frameIndex == 1) {
		memset(this->AccumulationData, 0, this->m_FinalImage->GetWidth() * this->m_FinalImage->GetHeight() * sizeof(glm::vec4));
	}

	for (uint32_t y = 0; y < this->m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 1; x < this->m_FinalImage->GetWidth(); x+=2) {

			glm::vec4 color = this->PerPixel(x, y);
			this->AccumulationData[x + y * this->m_FinalImage->GetWidth()] += color;
			this->AccumulationData[(x - 1) + y * this->m_FinalImage->GetWidth()] += color;

			glm::vec4 accumulatedColor = this->AccumulationData[x + y * this->m_FinalImage->GetWidth()];
			accumulatedColor /= this->frameIndex;

			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
			uint32_t RGBA = Utils::ConvertToRGBA(accumulatedColor);
			this->imageData[x + y * this->m_FinalImage->GetWidth()] = RGBA;
			this->imageData[(x - 1) + y * this->m_FinalImage->GetWidth()] = RGBA;
		}
	}

	this->m_FinalImage->SetData(this->imageData);

	if (this->settings.Accumulate) {
		this->frameIndex++;
	}
	else {
		this->frameIndex = 1;
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

		if (payload.HitDist < 0.0f) {
			glm::vec3 skycolor = glm::vec3(0.6f, 0.7f, 0.9f);
			color += skycolor * multiplier;
			break;
		}

		glm::vec3 lightDir = glm::normalize(this->lightDir);
		float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // == cos(angle)

		const Sphere& sphere = this->ActiveScene->spheres[payload.ObjectIndex];
		const Material& material = this->ActiveScene->materials[sphere.material_index];

		glm::vec3 sphereColor = material.Albedo;
		sphereColor *= lightIntensity;

		color += sphereColor * multiplier;

		multiplier *= 0.5f;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::reflect(ray.Direction, 
			payload.WorldNormal + material.roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
	}

	return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDist, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.HitDist = hitDist;
	payload.ObjectIndex = objectIndex;


	const Sphere& closestSphere = ActiveScene->spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.pos;
	payload.WorldPosition = origin + ray.Direction * hitDist;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.pos;

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

	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0

	// a = ray origin
	// b = ray direction
	// r = radius of sphere
	// t = hit distance

	int closestSphere = -1;
	float hitDist = std::numeric_limits<float>::max();

	for (int i = 0; i < this->ActiveScene->spheres.size(); i++) {

		const Sphere& sphere = this->ActiveScene->spheres[i];
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

		if (closestT > 0.0f && closestT < hitDist) {
			hitDist = closestT;
			closestSphere = i;
		}
	}	
	
	if (closestSphere < 0) {
		return this->Miss(ray);
	}

	return this->ClosestHit(ray, hitDist, closestSphere);
}

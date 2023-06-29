#pragma once

#include <glm/glm.hpp>
#include <vector>

class Camera
{
public:
	Camera(float verticalFOV, float nearClip, float farClip);

	bool OnUpdate(float ts, bool hasObjects);
	void OnResize(uint32_t width, uint32_t height);
	void RecalculateProjection();
	void RecalculateView();
	
	glm::vec3 CalculateRayDirection(uint32_t u, uint32_t v) const;

	const glm::mat4& GetProjection() const { return m_Projection; }
	const glm::mat4& GetInverseProjection() const { return m_InverseProjection; }
	const glm::mat4& GetView() const { return m_View; }
	const glm::mat4& GetInverseView() const { return m_InverseView; }

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetDirection() const { return m_ForwardDirection; }

	const float GetVerticalFOV() const { return m_VerticalFOV; }
	const float GetNearClip() const { return m_NearClip; }
	const float GetFarClip() const { return m_FarClip; }

	void SetPosition(glm::vec3 position) { m_Position = position; }
	void SetDirection(glm::vec3 direction) { m_ForwardDirection = direction; }
	void SetVerticalFOV(float verticalFOV) { m_VerticalFOV = verticalFOV; }
	void SetNearClip(float nearClip) { m_NearClip = nearClip; }
	void SetFarClip(float farClip) { m_FarClip = farClip; }

	float GetRotationSpeed();

private:
	glm::mat4 m_Projection{ 1.0f };
	glm::mat4 m_View{ 1.0f };
	glm::mat4 m_InverseProjection{ 1.0f };
	glm::mat4 m_InverseView{ 1.0f };

	float m_VerticalFOV = 45.0f;
	float m_NearClip = 0.1f;
	float m_FarClip = 100.0f;

	glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_ForwardDirection{ 0.0f, 0.0f, 0.0f };

	glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};

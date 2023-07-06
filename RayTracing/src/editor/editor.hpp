#pragma once

#include "imgui.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "../renderer/renderer.hpp"
#include "../renderer/camera.hpp"
#include "../scene/sceneinfo.hpp"
#include "../scene/scene.hpp"
#include "../scene/export.hpp"
#include "../global.h"

#include <glm/gtc/type_ptr.hpp>

class Editor {
	
public:
	Editor();

	void OnUIRender();
	void OnUpdate(float ts);

private:
	void render();
	void SceneUI(bool& sceneMoved);
	void MaterialUI(bool& sceneMoved);
	void OptionsUI();
	void SettingsUI(bool& sceneMoved);

private:
	// object ui in SceneUI
	void SphereUI(bool& sceneMoved, std::vector<int>& spheres_todelete);
	void BoxUI(bool& sceneMoved, std::vector<int>& boxes_todelete);

private:
	Renderer renderer;

	Camera camera;
	Scene scene;

	int m_ViewportWidth = 1200, m_ViewportHeight = 800;
	
	float imageScale = 0.25f;

	float last_render_time = 0;

	// external classes
	SceneInfo sceneinfo = SceneInfo();
};

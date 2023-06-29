#pragma once

#include "imgui.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "../renderer/renderer.hpp"
#include "../renderer/camera.hpp"
#include "../scene/export.hpp"
#include "../scene/sceneinfo.hpp"
#include "../scene/scene.hpp"
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
	Renderer renderer;

	Camera camera;
	Scene scene;

	int m_ViewportWidth = 1200, m_ViewportHeight = 900;
	
	// set the scale to be smaller when doing the real time
	// when doing final image render, set scale to 1.0f
	float scale = 0.5f;

	float last_render_time = 0;

	// external classes
	Export exportScene = Export();
	SceneInfo sceneinfo = SceneInfo();
};

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "renderer.h"
#include "camera.h"
#include "export.h"
#include "sceneinfo.h"

#include <glm/gtc/type_ptr.hpp>

class RayTracing : public Walnut::Layer
{
public:
	RayTracing()
		: camera(45.0f, 0.1f, 100.0f)
	{
		scene.name = std::system("python ..\\Helper\\date_time.py");

		Material& material0 = this->scene.materials.emplace_back();
		material0.Albedo = { 0.2f, 0.3f, 1.0f };
		material0.roughness = 0.1f;
		material0.name = "Material0";

		{
			Sphere sphere;
			sphere.name = "Sphere 0";
			sphere.pos = { 0.0f, 0.0f, 0.0f };
			sphere.radius = 0.5f;
			sphere.material_index = 0;
			this->scene.spheres.push_back(sphere);
		}
	}

	virtual void OnUpdate(float ts) override {
		if (!this->renderer.GetRealisticRendering()) {
			if (this->camera.OnUpdate(ts)) {
				this->renderer.SetSceneMoved(true);
				this->renderer.SetCameraMoved(true);
			}
			else {
				this->renderer.SetSceneMoved(false);
				this->renderer.SetCameraMoved(false);
			}
		}

		if (this->renderer.GetFinishedRealistic()) {
			this->exportScene.reset();
			this->exportScene.SetIsExport(true);
			this->renderer.SetFinishRealisticAndExport();
		}

		if (this->exportScene.GetIsExport()) {
			// keep on writing to the ppm file
			// until GetFinishedExport is true
			if (!exportScene.GetFinishedExport()) {
				this->exportScene.ExportImage(
					this->renderer.GetImageData(),
					this->renderer.GetImageWidth(),
					this->renderer.GetImageHeight()
				);
			}
			else {
				this->exportScene.SetIsExport(false);
			}
		}
	}

	virtual void OnUIRender() override
	{
		bool sceneMoved = this->renderer.GetSceneMoved();
		// if camera moved, scene moved is
		// automatically set to true no matter what
		if (this->renderer.GetCameraMoved() || this->renderer.GetRealisticRendering()) {
			sceneMoved = true;
		}
		else {
			sceneMoved = false;
		}

		ImGui::Begin("Options");

		// exportScene
		if (this->renderer.GetRealisticRendering()) {
			ImGui::Text("Rendering Realistic Image...");
		}

		if (!this->renderer.GetRealisticRendering() && !this->exportScene.GetIsExport()) {
			if (ImGui::Button("Realistic Rendering")) {
				this->renderer.realisticRender();
			}

			ImGui::Separator();
			ImGui::Separator();

			if (ImGui::BeginCombo("Format", this->exportScene.GetCurrentFormat())) {
				char* allFormats[2];
				char** formatsPtr = this->exportScene.GetFormats();

				for (int i = 0; i < 2; i++) {
					allFormats[i] = *(formatsPtr + i);
				}

				for (int n = 0; n < IM_ARRAYSIZE(allFormats); n++) {
					bool isSelected = (this->exportScene.GetCurrentFormat() == this->exportScene.GetFormats()[n]);
					if (ImGui::Selectable(this->exportScene.GetFormats()[n], isSelected)) {
						this->exportScene.setFormat(this->exportScene.GetFormats()[n]);
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}	
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Export")) {
				this->exportScene.reset();
				this->exportScene.SetIsExport(true);
			}

			ImGui::Separator();
			ImGui::Separator();

			if (ImGui::Button("Save scene info")) {
				this->sceneinfo.write(this->scene.spheres, this->scene.materials);
			}
			if (this->sceneinfo.GetFinishedSave()) {
				ImGui::Text("Successfully saved scene");
			}
		}

		if (this->exportScene.GetIsExport()) {
			this->exportScene.updatePercentage(this->renderer.GetImageWidth(), this->renderer.GetImageHeight());	
			double percentage = this->exportScene.GetPercentage();
			ImGui::Text("Exporting at %.2f", percentage);
		}

		ImGui::End();

		if (!this->renderer.GetRealisticRendering() && !this->exportScene.GetIsExport()) {
			// options
			ImGui::Begin("Settings");
			ImGui::Text("Last Render: %.3fms", this->last_render_time);

			ImGui::Separator();
			ImGui::Separator();

			if (ImGui::DragFloat("Multiplier", &(this->renderer.GetBrightness()), 0.05f, 0.1f, 1.0f)) {
				sceneMoved = true;
			}

			ImGui::Separator();
			ImGui::Separator();

			if (ImGui::DragInt("Width", &m_ViewportWidth, 1.0f, 200, 1500)) {
				sceneMoved = true;
			}

			if (ImGui::DragInt("Height", &m_ViewportHeight, 1.0f, 200, 1170)) {
				sceneMoved = true;
			}

			ImGui::End();

			// entities
			// settings
			std::vector<int> spheres_todelete = {};
			ImGui::Begin("Scene");

			ImGui::Text("Light");
			if (ImGui::DragFloat3("Light Direction", glm::value_ptr(this->renderer.GetLightDir()), 0.1f, -1.0f, 1.0f)) {
				sceneMoved = true;
			}

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("Background");
			if (ImGui::ColorEdit3("Sky", glm::value_ptr(this->renderer.GetSkycolor()))) {
				sceneMoved = true;
			}

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("Objects");

			if (ImGui::Button("New Sphere")) {

				glm::vec3 camera_pos = this->camera.GetPosition();
				glm::vec3 camera_dir = this->camera.GetDirection();

				Sphere sphere;
				sphere.name = "Sphere " + std::to_string(this->scene.spheres.size());
				sphere.pos = {
					camera_pos.x + camera_dir.x * 2,
					camera_pos.y + camera_dir.y * 2,
					camera_pos.z + camera_dir.z * 2
				};
				sphere.radius = 0.5f;
				sphere.material_index = 0;

				this->scene.spheres.push_back(sphere);

				sceneMoved = true;
			}


			for (int i = 0; i < scene.spheres.size(); i++) {
				ImGui::PushID(i);

				ImGui::Separator();
				ImGui::Separator();

				Sphere& sphere = scene.spheres[i];
				ImGui::Text(sphere.name.c_str());

				if (ImGui::DragFloat3("Position", glm::value_ptr(sphere.pos), 0.1f)) {
					sceneMoved = true;
				}

				if (ImGui::DragFloat("Radius", &sphere.radius, 0.1f, 0.0f)) {
					sceneMoved = true;
				}

				if (ImGui::DragInt("Material", &sphere.material_index, 1.0f, 0, int(this->scene.materials.size()) - 1)) {
					sceneMoved = true;
				}

				if (ImGui::Button("Delete Sphere")) {
					spheres_todelete.push_back(i);
					sceneMoved = true;
				}

				ImGui::PopID();
			}

			for (int index : spheres_todelete) {
				this->scene.spheres.erase(this->scene.spheres.begin() + index);
			}

			ImGui::End();

			ImGui::Begin("Materials");

			if (ImGui::Button("New Material")) {

				Material& material = this->scene.materials.emplace_back();
				material.Albedo = { 0.2f, 0.3f, 1.0f };
				material.roughness = 1.0f;
				material.name = "Material" + std::to_string(this->scene.materials.size() - 1);

				sceneMoved = true;
			}

			for (int i = 0; i < this->scene.materials.size(); i++) {
				ImGui::PushID(i);

				ImGui::Separator();
				ImGui::Separator();

				Material& material = this->scene.materials[i];

				ImGui::Text((material.name).c_str());

				if (ImGui::ColorEdit3("Color", glm::value_ptr(material.Albedo))) {
					sceneMoved = true;
				}

				if (ImGui::DragFloat("Roughness", &material.roughness, 0.1f, 0.0f, 1.0f)) {
					sceneMoved = true;
				}

				ImGui::PopID();
			}

			ImGui::End();
		}

		// viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		auto image = this->renderer.get_final_image();
		if (image) {
			ImGui::Image(image->GetDescriptorSet(), { float(image->GetWidth()), float(image->GetHeight()) },
				ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
		ImGui::PopStyleVar();

		this->renderer.SetSceneMoved(sceneMoved);
		if (sceneMoved) {
			this->sceneinfo.SetFinishedSave(false);
		}

		this->render();
	}

	void render() {
		if (!this->exportScene.GetIsExport()) {

			Walnut::Timer timer;

			if (this->renderer.on_resize(this->m_ViewportWidth, this->m_ViewportHeight)) {
				this->renderer.SetSceneMoved(true);
			}

			this->camera.OnResize(this->m_ViewportWidth, this->m_ViewportHeight);

			this->renderer.render(this->scene, this->camera);

			last_render_time = timer.ElapsedMillis();
		}
	}

private:
	Renderer renderer;

	Camera camera;
	Scene scene;

	int m_ViewportWidth = 1200, m_ViewportHeight = 965;

	float last_render_time = 0;

	// classes for saving the scene (jpg, png, ini, etc)
	Export exportScene = Export();
	SceneInfo sceneinfo = SceneInfo();
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "EvTech - Realistic 3D Art - v0.1";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<RayTracing>();

	/*
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	*/
	return app;
}
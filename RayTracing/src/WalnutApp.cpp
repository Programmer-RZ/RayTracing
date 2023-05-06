#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "renderer.h"
#include "camera.h"
#include "export.h"

#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: camera(45.0f, 0.1f, 100.0f)
	{
		std::system("python ..\\Helper\\date_time.py");

		std::ifstream file("..\\Data\\datetime.txt");
		std::string datetime;
		while (std::getline(file, datetime)) {
			scene.name = datetime;
		}

		Material& material0 = this->scene.materials.emplace_back();
		material0.Albedo = { 0.2f, 0.3f, 1.0f };
		material0.roughness = 0.1f;
		material0.name = "Material1";

		Material& material1 = this->scene.materials.emplace_back();
		material1.Albedo = { 1.0f, 0.0f, 1.0f };
		material1.roughness = 0.5f;
		material1.name = "Material0";

		{
			Sphere sphere;
			sphere.name = "Ground";
			sphere.pos = { 0.0f, -50.5f, 0.0f };
			sphere.radius = 50.0f;
			sphere.material_index = 0;
			this->scene.spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.name = "Sphere 1";
			sphere.pos = { 0.0f, 0.0f, 0.0f };
			sphere.radius = 0.5f;
			sphere.material_index = 1;
			this->scene.spheres.push_back(sphere);
		}
	}

	virtual void OnUpdate(float ts) override {
		if (!this->renderer.GetRealisticRendering()) {
			if (this->camera.OnUpdate(ts)) {
				this->renderer.resetFrameIndex();
			}
		}
	}

	virtual void OnUIRender() override
	{
		bool reset_Accumulation = false;

		ImGui::Begin("Options");

		// export
		if (this->renderer.GetRealisticRendering()) {
			ImGui::Text("Rendering Realistic Image...");
		}

		if (!this->renderer.GetRealisticRendering()) {
			if (ImGui::Button("Realistic Rendering")) {
				this->renderer.realisticRender();
			}

			if (ImGui::Button("Export as PNG")) {
				this->exportPNG.reset();
				this->exportPNG.SetIsExport(true);
			}
		}

		if (this->exportPNG.GetIsExport()) {
			int percentage = this->exportPNG.GetPercentage(this->renderer.GetImageWidth(), this->renderer.GetImageHeight());
			ImGui::Text("Exporting at %i", percentage);
		}

		ImGui::End();

		if (!this->renderer.GetRealisticRendering() && !this->exportPNG.GetIsExport()) {
			// options
			ImGui::Begin("Settings");
			ImGui::Text("Last Render: %.3fms", this->last_render_time);

			ImGui::Separator();
			ImGui::Separator();

			if (ImGui::DragFloat("Brightness", &(this->renderer.GetBrightness()), 0.1f, 0.1f, 1.0f)) {
				reset_Accumulation = true;
			}

			ImGui::Separator();
			ImGui::Separator();

			if (ImGui::DragInt("Width", &m_ViewportWidth, 1.0f, 200, 1500)) {
				reset_Accumulation = true;
			}

			if (ImGui::DragInt("Height", &m_ViewportHeight, 1.0f, 200, 1170)) {
				reset_Accumulation = true;
			}

			ImGui::End();

			// entities
			// settings
			std::vector<int> spheres_todelete = {};
			ImGui::Begin("Scene");

			ImGui::Text("Light");
			if (ImGui::DragFloat3("Light Direction", glm::value_ptr(this->renderer.GetLightDir()), 0.1f, 0.0f, 1.0f)) {
				reset_Accumulation = true;
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

				reset_Accumulation = true;
			}


			for (int i = 0; i < scene.spheres.size(); i++) {
				ImGui::PushID(i);

				ImGui::Separator();
				ImGui::Separator();

				Sphere& sphere = scene.spheres[i];
				ImGui::Text(sphere.name.c_str());

				if (ImGui::DragFloat3("Position", glm::value_ptr(sphere.pos), 0.1f)) {
					reset_Accumulation = true;
				}

				if (ImGui::DragFloat("Radius", &sphere.radius, 0.1f, 0.0f)) {
					reset_Accumulation = true;
				}

				if (ImGui::DragInt("Material", &sphere.material_index, 1.0f, 0, int(this->scene.materials.size()) - 1)) {
					reset_Accumulation = true;
				}

				if (ImGui::Button("Delete Sphere")) {
					spheres_todelete.push_back(i);
					reset_Accumulation = true;
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

				reset_Accumulation = true;
			}

			for (int i = 0; i < this->scene.materials.size(); i++) {
				ImGui::PushID(i);

				ImGui::Separator();
				ImGui::Separator();

				Material& material = this->scene.materials[i];

				ImGui::Text((material.name).c_str());

				if (ImGui::ColorEdit3("Color", glm::value_ptr(material.Albedo))) {
					reset_Accumulation = true;
				}

				if (ImGui::DragFloat("Roughness", &material.roughness, 0.1f, 0.0f, 1.0f)) {
					reset_Accumulation = true;
				}

				if (ImGui::DragFloat("Metallic", &material.metallic, 0.1f, 0.0f, 1.0f)) {
					reset_Accumulation = true;
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

		if (reset_Accumulation) {
			this->renderer.resetFrameIndex();
		}

		this->render();

		if (this->renderer.GetFinishedRealistic()) {
			this->exportPNG.reset();
			this->exportPNG.SetIsExport(true);
			this->renderer.SetFinishRealisticAndExport();
		}

		if (this->exportPNG.GetIsExport()) {
			if (!exportPNG.GetFinishedExport()) {
				this->export_as_PNG();
			}
			else {
				this->exportPNG.SetIsExport(false);
			}
		}
	}

	void export_as_PNG() {
		// export
		exportPNG.ExportImage(
			scene.name,
			this->renderer.GetImageData(),
			this->renderer.GetImageWidth(),
			this->renderer.GetImageHeight()
		);
	}

	void render() {
		if (!this->exportPNG.GetIsExport()) {
			Walnut::Timer timer;

			this->renderer.on_resize(this->m_ViewportWidth, this->m_ViewportHeight);
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

	Export exportPNG = Export();
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "RayTracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();

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
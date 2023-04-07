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
	}

	virtual void OnUpdate(float ts) override {
		this->camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		// options
		ImGui::Begin("Settings");
		ImGui::Text("Last Render: %.3fms", this->last_render_time);
	
		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		ImGui::Separator();
		ImGui::Separator();
		// export
		if (ImGui::Button("Export as PNG")) {
			this->export_as_PNG();
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		ImGui::Separator();
		ImGui::Separator();

		// save as
		if (ImGui::Button("Save scene info")) {

		}
		ImGui::End();

		// entities
		// settings
		std::vector<int> spheres_todelete = {};
		ImGui::Begin("Scene");

		if (ImGui::Button("New Sphere")) {

			glm::vec3 camera_pos = this->camera.GetPosition();
			glm::vec3 camera_dir = this->camera.GetDirection();

			Sphere sphere;
			sphere.name = "Sphere " + std::to_string(this->scene.spheres.size() + 1);
			sphere.pos = {
				camera_pos.x + camera_dir.x * 2,
				camera_pos.y + camera_dir.y * 2,
				camera_pos.z + camera_dir.z * 2
			};
			sphere.radius = 0.5f;
			sphere.Albedo = { 0.2f, 0.3f, 1.0f };

			this->scene.spheres.push_back(sphere);
		}


		for (int i = 0; i < scene.spheres.size(); i++) {
			ImGui::PushID(i);

			ImGui::Separator();
			ImGui::Separator();

			Sphere& sphere = scene.spheres[i];
			ImGui::Text(sphere.name.c_str());
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.pos), 0.1f);
			ImGui::DragFloat("Radius", &sphere.radius, 0.1f);
			ImGui::ColorEdit3("Color", glm::value_ptr(sphere.Albedo));

			if (ImGui::Button("Delete Sphere")) {
				spheres_todelete.push_back(i);
			}

			ImGui::PopID();
		}

		for (int index : spheres_todelete) {
			this->scene.spheres.erase(this->scene.spheres.begin() + index);
		}

		ImGui::End();

		// viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		this->m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		this->m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = this->renderer.get_final_image();
		if (image) {
			ImGui::Image(image->GetDescriptorSet(), { float(image->GetWidth()), float(image->GetHeight()) },
				ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
		ImGui::PopStyleVar();

		this->render();
	}

	void export_as_PNG() {
		// export
		Export::ExportImage(
			scene.name,
			this->renderer.GetImageData(),
			this->renderer.GetImageWidth(),
			this->renderer.GetImageHeight()
		);
	}

	void save_scene() {
		
	}

	void render() {
		Walnut::Timer timer;

		this->renderer.on_resize(this->m_ViewportWidth, this->m_ViewportHeight);
		this->camera.OnResize(this->m_ViewportWidth, this->m_ViewportHeight);
		this->renderer.render(this->scene, this->camera);
		
		last_render_time = timer.ElapsedMillis();
	}

private:
	Renderer renderer;
	Camera camera;
	Scene scene;

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float last_render_time = 0;
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
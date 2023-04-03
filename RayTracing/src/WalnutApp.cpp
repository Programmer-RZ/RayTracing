#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "renderer.h"
#include "camera.h"

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: camera(45.0f, 0.1f, 100.0f)
	{}

	virtual void OnUpdate(float ts) override {
		this->camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render: %.3fms", this->last_render_time);

		if (ImGui::Button("Render")) {
			this->render();
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

	void render() {
		Walnut::Timer timer;

		this->renderer.on_resize(this->m_ViewportWidth, this->m_ViewportHeight);
		this->camera.OnResize(this->m_ViewportWidth, this->m_ViewportHeight);
		this->renderer.render(this->camera);
		
		last_render_time = timer.ElapsedMillis();
	}

private:
	Renderer renderer;

	Camera camera;

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float last_render_time = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "RayTracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
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
	return app;
}
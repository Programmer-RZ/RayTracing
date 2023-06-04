#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "editor/editor.h"

class RayTracing : public Walnut::Layer
{
public:
	RayTracing() = default;

	virtual void OnUpdate(float ts) override {
		this->editor.OnUpdate(ts);
	}

	virtual void OnUIRender() override {
		this->editor.OnUIRender();
	}

private:
	Editor editor = Editor();
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Realistic 3D Art";

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
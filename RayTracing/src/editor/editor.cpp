#include "editor.h"

Editor::Editor()
	: camera(45.0f, 0.1f, 100.0f)
{
	//std::system(CREATESCENENAME_SCRIPT);
	this->sceneinfo.read(this->scene, this->camera, this->m_ViewportWidth, this->m_ViewportHeight);

	/*
	Material& material0 = this->scene.materials.emplace_back();
	material0.Albedo = { 0.2f, 0.3f, 1.0f };
	material0.roughness = 0.1f;
	material0.name = "Material0";
	material0.id = 0;
	*/
}

void Editor::OnUpdate(float ts) 
{
	if (!this->renderer.GetRealisticRendering()) {
		bool hasObjects = (
			this->scene.spheres.size() != 0
			||
			this->scene.cubes.size() != 0
			);
		if (this->camera.OnUpdate(ts, hasObjects)) {
			this->renderer.SetCameraMoved(true);
		}
		else {
			this->renderer.SetCameraMoved(false);
		}
	}

	if (this->renderer.GetFinishedRealistic()) {
		this->exportScene.reset();
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

void Editor::OnUIRender()
{
	bool sceneMoved;
	// if camera moved, scene moved is
	// automatically set to true no matter what
	if (this->renderer.GetCameraMoved() || this->renderer.GetRealisticRendering()) {
		sceneMoved = true;
	}
	else {
		sceneMoved = false;
	}

	this->OptionsUI();

	if (!this->renderer.GetRealisticRendering() && !this->exportScene.GetIsExport()) {
		// settings
		this->SettingsUI(sceneMoved);

		// scene
		this->SceneUI(sceneMoved);

		// materials
		this->MaterialUI(sceneMoved);

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

	if (sceneMoved) {
		if (!this->renderer.GetRealisticRendering()) {
			// if realistic rendering
			// do not interrupt the frame index
			this->renderer.resetFrameIndex();
		}

		this->sceneinfo.SetFinishedSave(false);
	}

	this->render();
}

void Editor::MaterialUI(bool& sceneMoved) {
	ImGui::Begin("Materials");

	if (ImGui::Button("New Material")) {

		Material& material = this->scene.materials.emplace_back();
		material.Albedo = { 0.2f, 0.3f, 1.0f };
		material.roughness = 1.0f;
		material.name = "Material" + std::to_string(this->scene.materials.size() - 1);
		material.id = this->scene.materials.size() - 1;

		sceneMoved = true;
	}

	// holds the indexes of material that the user deleted
	std::vector<int> materials_todelete = {};
	for (int i = 0; i < this->scene.materials.size(); i++) {
		ImGui::PushID(i);
		ImGui::Separator();

		Material& material = this->scene.materials[i];

		if (ImGui::CollapsingHeader(material.name.c_str())) {

			if (ImGui::ColorEdit3("Color", glm::value_ptr(material.Albedo))) {
				sceneMoved = true;
			}

			if (ImGui::DragFloat("Roughness", &material.roughness, 0.1f, 0.0f, 1.0f)) {
				sceneMoved = true;
			}

			if (ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.05f, 0.0f, FLT_MAX)) {
				sceneMoved = true;
			}

			if (ImGui::Button("Delete material")) {
				sceneMoved = true;
				materials_todelete.push_back(i);
			}
		}
		ImGui::PopID();
	}
	// delete all the materials
	for (int index : materials_todelete) {
		// if materials only has one element
		// cannot delete it
		if (this->scene.materials.size() > 1 || this->scene.spheres.size() == 0) {
			this->scene.materials.erase(this->scene.materials.begin() + index);
		}
	}
	// check if any spheres hold the materials
	// if they do, set the sphere's material to the prev one
	for (int i = 0; i < this->scene.spheres.size(); i++) {
		Sphere& sphere = this->scene.spheres[i];
		if (sphere.material_index > this->scene.materials.size() - 1) {
			sphere.material_index = this->scene.materials.size() - 1;
			sceneMoved = true;
		}
	}

	ImGui::End();
}

void Editor::SceneUI(bool& sceneMoved) {
	ImGui::Begin("Scene");
	ImGui::Text(scene.name.c_str());
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 25));

	// appearance
	ImGui::Text("Apperance");

	ImGui::Separator();
	ImGui::Separator();

	ImGui::Text("Background");
	if (ImGui::ColorEdit3("Sky", glm::value_ptr(this->scene.skycolor))) {
		sceneMoved = true;
	}

	// blank space to seperate Appearance and Objects
	ImGui::Dummy(ImVec2(0, 25));

	// objects
	ImGui::Text("Objects");
	ImGui::Separator();
	ImGui::Separator();

	if (ImGui::BeginCombo("3D Shape", this->scene.selected_object)) {

		for (int n = 0; n < this->scene.num_of_avail_obj; n++) {
			bool isSelected = (this->scene.selected_object == this->scene.objects[n]);
			if (ImGui::Selectable(this->scene.objects[n], isSelected)) {
				this->scene.selected_object = this->scene.objects[n];
				sceneMoved = true;

				std::string currentObject = this->scene.selected_object;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("New") && this->scene.materials.size() != 0) {

		glm::vec3 camera_pos = this->camera.GetPosition();
		glm::vec3 camera_dir = this->camera.GetDirection();
		sceneMoved = true;

		if (static_cast<std::string>(this->scene.selected_object) == "Sphere") {
			this->scene.createNewSphere(camera_pos, camera_dir);
		}

	}

	// holds the indexes of sphere that the user deleted
	std::vector<int> spheres_todelete = {};
	for (int i = 0; i < scene.spheres.size(); i++) {
		ImGui::PushID(i);
		ImGui::Separator();

		Sphere& sphere = scene.spheres[i];

		if (ImGui::CollapsingHeader(sphere.name.c_str())) {
			if (ImGui::DragFloat3("Position", glm::value_ptr(sphere.pos), 0.1f)) {
				sceneMoved = true;
			}

			if (ImGui::DragFloat("Radius", &sphere.radius, 0.1f, 0.0f)) {
				sceneMoved = true;
			}

			if (ImGui::BeginCombo("Material", this->scene.materials[sphere.material_index].name.c_str())) {

				for (int n = 0; n < this->scene.materials.size(); n++) {
					bool isSelected = (this->scene.materials[sphere.material_index].id == this->scene.materials[n].id);
					if (ImGui::Selectable(this->scene.materials[n].name.c_str(), isSelected)) {
						sphere.material_index = n;
						sceneMoved = true;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}


			if (ImGui::Button("Delete Sphere")) {
				spheres_todelete.push_back(i);
				sceneMoved = true;
			}
		}
		ImGui::PopID();
	}

	for (int index : spheres_todelete) {
		this->scene.spheres.erase(this->scene.spheres.begin() + index);
	}

	ImGui::End();
}

void Editor::SettingsUI(bool& sceneMoved) {
	ImGui::Begin("Settings");
	ImGui::Text("Last Render: %.3fms", this->last_render_time);

	ImGui::Separator();
	ImGui::Separator();

	if (ImGui::DragInt("Width", &m_ViewportWidth, 1.0f, 200, 1500)) {
		sceneMoved = true;
	}

	if (ImGui::DragInt("Height", &m_ViewportHeight, 1.0f, 200, 1170)) {
		sceneMoved = true;
	}

	ImGui::End();
}

void Editor::OptionsUI() {
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
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
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
			this->sceneinfo.write(this->scene, this->camera, this->m_ViewportWidth, this->m_ViewportHeight);
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
}

void Editor::render() {
	if (!this->exportScene.GetIsExport()) {

		Walnut::Timer timer;

		this->renderer.on_resize(this->m_ViewportWidth, this->m_ViewportHeight);

		this->camera.OnResize(this->m_ViewportWidth, this->m_ViewportHeight);

		this->renderer.render(this->scene, this->camera, this->scene.skycolor);

		last_render_time = timer.ElapsedMillis();
	}
}
#include "editor.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

// material/object max name length
#define MAX_NAME_LENGTH 100

Editor::Editor()
	: camera(45.0f, 0.1f, 100.0f)
{
	this->sceneinfo.read(this->scene, this->camera, this->m_ViewportWidth, this->m_ViewportHeight);
}

void Editor::OnUpdate(float ts) 
{
	if (!this->renderer.IsRenderingFinalImage()) {
		bool hasObjects = (
			this->scene.spheres.size() != 0
			||
			this->scene.boxes.size() != 0
			);

		if (this->camera.OnUpdate(ts, hasObjects)) {
			this->renderer.SetCameraMoved(true);
		}
		else {
			this->renderer.SetCameraMoved(false);
		}
	}

	if (this->renderer.IsFinishedFinalImage()) {

		this->exportScene.ExportImage(
			this->renderer.GetImageData(),
			this->renderer.GetImageWidth(),
			this->renderer.GetImageHeight(),
			this->scene.name
		);
		
		this->imageScale = 0.5f;

		this->renderer.SetFinishedFinalImage();
	}
}

void Editor::OnUIRender()
{
	bool sceneMoved = false;

	if (!this->renderer.IsRenderingFinalImage()) {
		// options
		this->OptionsUI();

		// settings
		this->SettingsUI(sceneMoved);

		// scene
		this->SceneUI(sceneMoved);

		// materials
		this->MaterialUI(sceneMoved);

		if (this->renderer.IsCameraMoved()) {
			sceneMoved = true;
		}

	}

	// viewport
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");

	auto image = this->renderer.get_final_image();
	if (image) {
		ImGui::SetCursorPos((ImGui::GetWindowSize() - ImVec2(static_cast<float>(image->GetWidth())/this->imageScale, static_cast<float>(image->GetHeight())/this->imageScale)) * 0.5f);
		ImGui::Image(image->GetDescriptorSet(), { static_cast<float>(image->GetWidth())/this->imageScale, static_cast<float>(image->GetHeight())/this->imageScale },
			ImVec2(0, 1), ImVec2(1, 0)); // flip the image
	}

	ImGui::End();
	ImGui::PopStyleVar();

	if (sceneMoved) {
		this->renderer.resetFrameIndex();
	}

	this->render();
}

void Editor::MaterialUI(bool& sceneMoved) {
	ImGui::Begin("Materials");

	if (ImGui::Button("New Material")) {

		glm::vec3 Albedo = { 0.8f, 0.8f, 0.8f };
		float roughness = 0.0f;
		float EmissionPower = 0.0f;


		int id = this->scene.materials.size();
		std::string name = "material" + std::to_string(id);

		Material material = Material(Albedo, roughness, EmissionPower, id, "lambertian", name);

		this->scene.materials.push_back(material);
	}

	// holds the indexes of material that the user deleted
	std::vector<int> materials_todelete = {};
	for (int i = 0; i < this->scene.materials.size(); i++) {
		ImGui::Separator();

		Material& material = this->scene.materials[i];

		char tmp[MAX_NAME_LENGTH] = "";
		strcat_s(tmp, material.name.c_str());

		strcat_s(tmp, "###");
		strcat_s(tmp, std::to_string(material.id).c_str());

		if (ImGui::CollapsingHeader(tmp)) {
			ImGui::PushID(material.id);

			char newname[MAX_NAME_LENGTH] = "";
			strcat_s(newname, material.name.c_str());
			ImGui::InputText("Name", newname, 20);

			if (newname[0] != 0) {
				// newname is not empty
				material.name = newname;
			}

			if (ImGui::ColorEdit3("Color", glm::value_ptr(material.Albedo))) { sceneMoved = true; }
			
			if (material.lighting == "reflect")
			{ if (ImGui::DragFloat("Roughness", &material.roughness, 0.1f, 0.0f, 1.0f)) { sceneMoved = true; } }
			
			if (material.lighting == "diffuse light")
			{ if (ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.05f, 0.0f, FLT_MAX)) { sceneMoved = true; } }

			if (ImGui::BeginCombo("Lighting", material.lighting.c_str())) {
				char* lightingMethods[3] = { "lambertian", "reflect" , "diffuse light" };
				for (int n = 0; n < 3; n++) {
					bool isSelected = (material.lighting == lightingMethods[n]);
					if (ImGui::Selectable(lightingMethods[n], isSelected)) {
						material.lighting = std::string(lightingMethods[n]);
						sceneMoved = true;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Delete material")) {
				sceneMoved = true;
				materials_todelete.push_back(i);
			}
			ImGui::PopID();
		}
	}
	// delete all the materials
	for (int index : materials_todelete) {
		// if materials only has one element
		// cannot delete it
		std::string name = this->scene.materials[index].name;
		if (this->scene.materials.size() > 1 || this->scene.spheres.size() == 0) {
			this->scene.materials.erase(this->scene.materials.begin() + index);
		}
	}

	// check if any spheres hold the materials
	for (int i = 0; i < this->scene.spheres.size(); i++) {
		Sphere& sphere = this->scene.spheres[i];
		if (sphere.material_index > this->scene.materials.size() - 1) {
			sphere.material_index = this->scene.materials.size() - 1;
			sceneMoved = true;
		}
	}

	// check if any boxes hold the materials
	for (int i = 0; i < this->scene.boxes.size(); i++) {
		Box& box = this->scene.boxes[i];
		if (box.material_index > this->scene.materials.size() - 1) {
			box.material_index = this->scene.materials.size() - 1;
			sceneMoved = true;
		}
	}

	ImGui::End();
}

void Editor::SceneUI(bool& sceneMoved) {
	ImGui::Begin("Scene");
	ImGui::Text(scene.name.c_str());
	
	ImGui::Dummy(ImVec2(0, 10));
	
	// sky color
	if (ImGui::ColorEdit3("Sky Color", glm::value_ptr(this->scene.skycolor))) { sceneMoved = true; }
	
	ImGui::Dummy(ImVec2(0, 10));

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
		std::string name = "";

		if (static_cast<std::string>(this->scene.selected_object) == "Sphere") {
			name = this->scene.createNewSphere(camera_pos, camera_dir);
		}
		if (static_cast<std::string>(this->scene.selected_object) == "Box") {
			name = this->scene.createNewBox(camera_pos, camera_dir);
		}

		sceneMoved = true;
	}

	// holds the indexes of objects the user deleted
	std::vector<int> spheres_todelete = {};
	std::vector<int> boxes_todelete = {};

	// sphere's ui
	if (this->scene.selected_object == this->scene.objects[0])
	{
		for (int i = 0; i < this->scene.spheres.size(); i++) {
			ImGui::Separator();

			Sphere& sphere = this->scene.spheres[i];

			char tmp[MAX_NAME_LENGTH] = "";
			strcat_s(tmp, sphere.name.c_str());

			strcat_s(tmp, "###");
			strcat_s(tmp, std::to_string(i).c_str());

			if (ImGui::CollapsingHeader(tmp)) {
				ImGui::PushID(i);

				char newname[MAX_NAME_LENGTH] = "";
				strcat_s(newname, sphere.name.c_str());
				ImGui::InputText("Name", newname, 20);

				if (newname[0] != 0) {
					// newname is not empty
					sphere.name = newname;
				}

				if (ImGui::DragFloat3("Position", glm::value_ptr(sphere.pos), 0.1f)) { sceneMoved = true; }

				if (ImGui::DragFloat("Radius", &sphere.radius, 0.1f, 0.0f)) { sceneMoved = true; }

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
				ImGui::PopID();
			}
		}
	}

	// box's ui
	else if (this->scene.selected_object == this->scene.objects[1])
	{
		for (int i = 0; i < this->scene.boxes.size(); i++) {
			ImGui::Separator();

			Box& box = this->scene.boxes[i];

			char tmp[MAX_NAME_LENGTH] = "";
			strcat_s(tmp, box.name.c_str());

			strcat_s(tmp, "###");
			strcat_s(tmp, std::to_string(i).c_str());

			if (ImGui::CollapsingHeader(tmp)) {
				ImGui::PushID(i);
				bool updateSides = false;

				char newname[MAX_NAME_LENGTH] = "";
				strcat_s(newname, box.name.c_str());
				ImGui::InputText("Name", newname, 20);

				if (newname[0] != 0) {
					// newname is not empty
					box.name = newname;
				}

				if (ImGui::DragFloat3("Corner 1", glm::value_ptr(box.box_min), 0.1f)) { sceneMoved = true; updateSides = true; }

				if (ImGui::DragFloat("Length", &box.length, 0.1f, 0.0f)) { sceneMoved = true; updateSides = true; }
				if (ImGui::DragFloat("Width", &box.width, 0.1f, 0.0f)) { sceneMoved = true; updateSides = true; }
				if (ImGui::DragFloat("Height", &box.height, 0.1f, 0.0f)) { sceneMoved = true; updateSides = true; }

				if (ImGui::BeginCombo("Material", this->scene.materials[box.material_index].name.c_str())) {

					for (int n = 0; n < this->scene.materials.size(); n++) {
						bool isSelected = (this->scene.materials[box.material_index].id == this->scene.materials[n].id);
						if (ImGui::Selectable(this->scene.materials[n].name.c_str(), isSelected)) {
							box.material_index = n;
							sceneMoved = true;
							updateSides = true;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (updateSides) {
					box.updateSides();
				}


				if (ImGui::Button("Delete Box")) {
					boxes_todelete.push_back(i);
					sceneMoved = true;
				}
				ImGui::PopID();
			}
		}
	}

	for (int index : spheres_todelete) {
		std::string name = this->scene.spheres[index].name;
		this->scene.spheres.erase(this->scene.spheres.begin() + index);

	}
	for (int index : boxes_todelete) {
		std::string name = this->scene.boxes[index].name;
		this->scene.boxes.erase(this->scene.boxes.begin() + index);

	}

	ImGui::End();
}

void Editor::SettingsUI(bool& sceneMoved) {
	ImGui::Begin("Settings");
	ImGui::Text("Last Render: %.3fms", this->last_render_time);

	ImGui::Separator();
	ImGui::Separator();

	if (ImGui::DragInt("Width", &m_ViewportWidth, 1.0f, 200, 1500)) { sceneMoved = true; }

	if (ImGui::DragInt("Height", &m_ViewportHeight, 1.0f, 200, 1170)) { sceneMoved = true; }

	ImGui::End();
}

void Editor::OptionsUI() {
	ImGui::Begin("Options");

	if (this->renderer.IsRenderingFinalImage()) {
		ImGui::Text("Rendering Final Image...");
	}

	if (ImGui::Button("Render and export final image")) {
		this->renderer.SetupFinalImage();
		
		this->imageScale = 1.0f;
	}

	ImGui::Separator();
	ImGui::Separator();

	if (ImGui::BeginCombo("Format", this->exportScene.GetCurrentFormat())) {

		char* allFormats[1];
		char** formatsPtr = this->exportScene.GetFormats();

		for (int i = 0; i < 1; i++) {
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
		this->exportScene.ExportImage(
			this->renderer.GetImageData(),
			this->renderer.GetImageWidth(),
			this->renderer.GetImageHeight(),
			this->scene.name
		);
	}

	ImGui::Separator();
	ImGui::Separator();

	if (ImGui::Button("Save scene info")) {
		this->sceneinfo.write(this->scene, this->camera, this->m_ViewportWidth, this->m_ViewportHeight);
	}

	ImGui::End();
}

void Editor::render() {
	Walnut::Timer timer;

	this->renderer.on_resize(this->m_ViewportWidth*this->imageScale, this->m_ViewportHeight*this->imageScale);

	this->camera.OnResize(this->m_ViewportWidth*this->imageScale, this->m_ViewportHeight*this->imageScale);

	this->renderer.render(this->scene, this->camera, this->scene.skycolor);

	last_render_time = timer.ElapsedMillis();
}
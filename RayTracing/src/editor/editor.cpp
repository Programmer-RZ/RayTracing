#include "editor.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

// material/object max name length
#define MAX_NAME_LENGTH 100

Editor::Editor() {
	this->SetTheme(false, 1.0f);
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

		Export::ExportImage(
			this->renderer.GetImageData(),
			this->renderer.GetImageWidth(),
			this->renderer.GetImageHeight()
		);
		
		this->imageScale = 0.25f;

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

		std::string tmp = material.name;
		tmp += "###";
		tmp += i;

		if (ImGui::CollapsingHeader(tmp.c_str())) {
			ImGui::PushID(material.id);

			char newname[MAX_NAME_LENGTH] = "";
			strcat_s(newname, material.name.c_str());
			ImGui::InputText("Name", newname, MAX_NAME_LENGTH);

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
	if (this->scene.selected_object == this->scene.objects[0]) {
		this->SphereUI(sceneMoved, spheres_todelete);
	}
	// box's ui
	else if (this->scene.selected_object == this->scene.objects[1]) {
		this->BoxUI(sceneMoved, boxes_todelete);
	}
	

	// delete the spheres
	for (int index : spheres_todelete) {
		std::string name = this->scene.spheres[index].name;
		this->scene.spheres.erase(this->scene.spheres.begin() + index);

	}
	// delete the boxes
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

	if (ImGui::DragInt("Width", &(this->ViewportWidth), 1.0f, 200, 1500)) { sceneMoved = true; }

	if (ImGui::DragInt("Height", &(this->ViewportHeight), 1.0f, 200, 1170)) { sceneMoved = true; }

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

	if (ImGui::Button("Import scene")) {
		this->scenedata.read(this->scene, this->camera, this->ViewportWidth, this->ViewportHeight);
		this->renderer.resetFrameIndex();
		this->render();
	}
	
	if (ImGui::Button("Save / Save to")) {
		this->scenedata.write(this->scene, this->camera, this->ViewportWidth, this->ViewportHeight);
	}

	ImGui::Separator();
	ImGui::Separator();
	
	if (ImGui::Button("Export image")) {

		Export::ExportImage(
			this->renderer.GetImageData(),
			this->renderer.GetImageWidth(),
			this->renderer.GetImageHeight()
		);
	}

	ImGui::End();
}

void Editor::render() {
	Walnut::Timer timer;

	this->renderer.on_resize(this->ViewportWidth*this->imageScale, this->ViewportHeight*this->imageScale);

	this->camera.OnResize(this->ViewportWidth*this->imageScale, this->ViewportHeight*this->imageScale);

	this->renderer.render(this->scene, this->camera, this->scene.skycolor);

	last_render_time = timer.ElapsedMillis();
}

void Editor::SphereUI(bool& sceneMoved, std::vector<int>& spheres_todelete) {
	// spheres ui in Scene tab
	for (int i = 0; i < this->scene.spheres.size(); i++) {
		ImGui::Separator();

		Sphere& sphere = this->scene.spheres[i];

		std::string tmp = sphere.name;
		// add the ###i
		// so when editing the name
		// it doesn't collapse
		tmp += "###";
		tmp += i;

		if (ImGui::CollapsingHeader(tmp.c_str())) {
			ImGui::PushID(i);

			char newname[MAX_NAME_LENGTH] = "";
			strcat_s(newname, sphere.name.c_str());
			ImGui::InputText("Name", newname, MAX_NAME_LENGTH);

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

void Editor::BoxUI(bool& sceneMoved, std::vector<int>& boxes_todelete) {
	for (int i = 0; i < this->scene.boxes.size(); i++) {
		ImGui::Separator();

		Box& box = this->scene.boxes[i];

		std::string tmp = box.name;
		tmp += "###";
		tmp += i;

		if (ImGui::CollapsingHeader(tmp.c_str())) {
			ImGui::PushID(i);
			bool updateSides = false;

			char newname[MAX_NAME_LENGTH] = "";
			strcat_s(newname, box.name.c_str());
			ImGui::InputText("Name", newname, MAX_NAME_LENGTH);

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



// imgui theme
// set ImGui theme
void Editor::SetTheme(bool darkmode, float alpha_) {
	// from https://gist.github.com/dougbinks/8089b4bbaccaaf6fa204236978d165a9#file-imguiutils-h-L9-L93
	
	ImGuiStyle& style = ImGui::GetStyle();
	
	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
		ImGuiCol_ ei = (ImGuiCol_)i;
		ImVec4& col = style.Colors[i];
		if(  (ImGuiCol_ModalWindowDimBg  != ei ) &&
			( ImGuiCol_NavWindowingDimBg != ei ) &&
			( col.w < 1.00f || ( ImGuiCol_FrameBg  == ei )
							|| ( ImGuiCol_WindowBg == ei ) 
							|| ( ImGuiCol_ChildBg  == ei ) ) )
		{
			col.w = alpha_ * col.w;
		}
	}
	
	style.ChildBorderSize = 1.0f;
	style.FrameBorderSize = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.WindowBorderSize = 0.0f;
	style.FrameRounding = 3.0f;
	style.Alpha = 1.0f;
	
	if (darkmode) { ImGui::StyleColorsDark(); }
	else { ImGui::StyleColorsLight(); }
}
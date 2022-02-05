#include "ModelEditor.h"
#include "FileDialog.h"
#include "ResourceManager.h"
#include "ImGui/imgui.h"
#include "IconsFontAwesome5.h"
#include "UI.h"
#include <filesystem>

namespace Glitter
{
	namespace Editor
	{
		ModelEditor::ModelEditor() : modelIndex{ -1 }, textureIndex{ -1 }, animationTimeline{ AnimationTimeline(UI::mTimelineWindow) }
		{
		}

		void ModelEditor::openModel()
		{
			std::string name;
			if (FileDialog::openFileDialog(FileType::Model, name))
				ResourceManager::loadModel(name);
		}

		void ModelEditor::openModelFolder()
		{
			std::string dir;
			if (FileDialog::openFolderDialog(dir))
			{
				if (!std::filesystem::exists(dir))
					return;

				for (const auto& file : std::filesystem::directory_iterator(dir))
				{
					std::string extension = file.path().extension().string();
					if (extension == ".model")
					{
						ResourceManager::loadModel(file.path().string());
					}
				}
			}
		}

		void ModelEditor::openTexture()
		{
			std::string name;
			if (FileDialog::openFileDialog(FileType::Texture, name))
				ResourceManager::loadTexture(name, TextureSlot::Diffuse);
		}

		void ModelEditor::openTextureFolder(const std::string& filepath)
		{
			std::string dir;
			if (FileDialog::openFolderDialog(dir))
			{
				if (!std::filesystem::exists(dir))
					return;

				for (const auto& file : std::filesystem::directory_iterator(dir))
				{
					std::string extension = file.path().extension().string();
					if (extension == ".dds")
					{
						ResourceManager::loadTexture(file.path().string(), TextureSlot::Diffuse);
					}
				}
			}
		}

		void ModelEditor::updateListview()
		{
			ImGuiTreeNodeFlags nrm = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth |
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;

			ImGuiTreeNodeFlags sel = nrm | ImGuiTreeNodeFlags_Selected;

			if (ImGui::Begin(UI::modelWindow))
			{
				UI::itemRowsBackground();
				
				std::vector<std::shared_ptr<ModelData>> models = ResourceManager::getModelList();
				for (int m = 0; m < models.size(); ++m)
				{
					std::string lbl = ICON_FA_CUBE;
					lbl.append(" " + models[m]->getName());
					ImGui::TreeNodeEx(lbl.c_str(), modelIndex == m ? sel : nrm);

					if (ImGui::IsItemActivated())
					{
						modelIndex = m;
						modelViewer.setModel(models[modelIndex]);
					}
				}
			}

			ImGui::End();

			if (ImGui::Begin(UI::texWindow))
			{
				UI::itemRowsBackground();

				std::vector<std::shared_ptr<TextureData>> textures = ResourceManager::getTextureList();
				for (int t = 0; t < textures.size(); ++t)
				{
					std::string lbl = ICON_FA_IMAGE;
					lbl.append(" " + textures[t]->getName());
					ImGui::TreeNodeEx(lbl.c_str(), textureIndex == t ? sel : nrm);

					if (ImGui::IsItemActivated())
					{
						textureIndex = t;
					}
				}
			}

			ImGui::End();
		}

		void ModelEditor::updateMenubar()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));
			ImGui::BeginMainMenuBar();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "Ctrl + O"))
					openModel();

				if (ImGui::MenuItem("Open Folder"))
					openModelFolder();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem(modelViewer.isPlaying() ? "Pause" : "Play"))
					modelViewer.togglePlayback();

				if (ImGui::MenuItem("Stop"))
					modelViewer.stopPlayback();

				if (ImGui::MenuItem("Replay"))
					modelViewer.replay();

				ImGui::Separator();
			}

			ImGui::PopStyleVar();
			ImGui::EndMainMenuBar();
		}

		void ModelEditor::update(Renderer* renderer, float deltaT)
		{
			updateMenubar();
			updateListview();

			modelViewer.update(renderer, deltaT);
			animationTimeline.update();
		}
	}
}
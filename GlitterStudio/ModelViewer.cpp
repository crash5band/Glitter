#include "ModelViewer.h"
#include "ResourceManager.h"
#include "IconsFontAwesome5.h"
#include "UI.h"
#include "UiHelper.h"
#include "FileGUI.h"
#include <filesystem>

namespace Glitter
{
	namespace Editor
	{
		ModelViewer::ModelViewer() :
			index{ -1 }, playing{ false }, loop{ true }, drawGrid{ true }, playbackSpeed{ 1.0 }
		{
			selectedModel = nullptr;
			time = maxTime = 0;
		}

		void ModelViewer::open()
		{
			std::string name;
			if (FileGUI::openFileGUI(FileType::Model, name))
				ResourceManager::loadModel(name);
		}

		void ModelViewer::openFolder()
		{
			std::string dir;
			if (FileGUI::openFolderGUI(dir))
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

		void ModelViewer::togglePlayback()
		{
			if (selectedModel)
				playing ^= true;
		}

		void ModelViewer::stopPlayback()
		{
			playing = time = 0;
		}

		void ModelViewer::replay()
		{
			stopPlayback();
			togglePlayback();
		}

		bool ModelViewer::isPlaying()
		{
			return playing;
		}

		bool ModelViewer::isLoop()
		{
			return loop;
		}

		void ModelViewer::setModel(std::shared_ptr<ModelData> &model)
		{
			selectedModel = model;
		}

		void ModelViewer::updatePreview(Renderer* renderer, float deltaT)
		{
			viewport.use();

			if (drawGrid)
				renderer->drawGrid(viewport);

			if (selectedModel)
			{
				time += deltaT * 60.0f * playbackSpeed * playing;
				renderer->drawMesh(selectedModel, viewport, time);
			}

			viewport.end();
		}

		void ModelViewer::modelProperties()
		{
			if (ImGui::Begin(UI::mPropertiesWindow))
			{
				if (!selectedModel)
				{
					ImGui::TextWrapped("Select a model from the models list to adjust preview properties.");
					ImGui::End();
					return;
				}

				ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

				if (ImGui::TreeNodeEx(ICON_FA_ARROWS_ALT " Transform", treeFlags))
				{
					Transform t = selectedModel->getTransform();

					float p3[3]{ t.position.x, t.position.y, t.position.z };
					if (ImGui::DragFloat3("Position", p3, 0.1f, 0.0f, 0.0f, "%g"))
						selectedModel->move(Glitter::Vector3(p3[0], p3[1], p3[2]));

					Glitter::Vector3 rot = t.rotation.toEulerDegrees();
					float r3[3]{ rot.x, rot.y, rot.z };

					if (ImGui::DragFloat3("Rotation", r3, 0.1f, 0.0f, 0.0f, "%g"))
					{
						Glitter::Quaternion q;
						rot.x = r3[0];
						rot.y = r3[1];
						rot.z = r3[2];
						q.fromEulerDegrees(rot);
						selectedModel->rotate(q);
					}

					float s3[3]{ t.scale.x, t.scale.y, t.scale.z };
					if (ImGui::DragFloat3("Scale", s3, 0.1f, 0.0f, 0.0f, "%g"))
						selectedModel->scale(Glitter::Vector3(s3[0], s3[1], s3[2]));

					ImGui::TreePop();
				}

				/*
				if (ImGui::TreeNodeEx("Materials", treeFlags))
				{
					std::vector<std::shared_ptr<Glitter::Material>> materials = selectedModel->getMaterials();
					std::vector<std::string> matNames;
					matNames.reserve(materials.size());
					for (int i = 0; i < materials.size(); ++i)
						matNames.push_back(materials[i]->getName());

					static int selectedIndex = 0;
					if (selectedIndex >= matNames.size())
						selectedIndex = matNames.size() - 1;

					std::string comboLabel = matNames[selectedIndex];
					if (ImGui::BeginCombo("Material", comboLabel.c_str()))
					{
						for (int n = 0; n < materials.size(); ++n)
						{
							const bool selected = selectedIndex == n;
							if (ImGui::Selectable(matNames[n].c_str(), selected))
							{
								selectedIndex = n;
							}

							if (selected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}

					if (selectedIndex >= matNames.size())
						selectedIndex = matNames.size() - 1;

					for (int i = 0; i < materials[selectedIndex]->getParameters().size(); ++i)
					{
						Glitter::MaterialParam* param = materials[selectedIndex]->getParameterByIndex(i);
						float c4[4]{ param->color.r, param->color.g, param->color.b, param->color.a };

						ImGui::Text(param->name.c_str());

						std::string lbl = "##" + param->name;
						if (ImGui::DragFloat4(lbl.c_str(), c4, 0.1f, 0.0f, 0.0f, "%g"))
							param->color = Glitter::Color(c4[0], c4[1], c4[2], c4[3]);

						ImGui::Separator();
					}

					ImVec2 windowSize = ImGui::GetCurrentWindow()->Size;
					if (ImGui::TreeNodeEx("Textures", treeFlags))
					{
						std::vector<Glitter::Texture*> textures = materials[selectedIndex]->getTextures();
						static int selectedTexture = 0;

						if (selectedTexture >= textures.size())
							selectedTexture = textures.size() - 1;

						if (ImGui::BeginChild("##texture_select", ImVec2(windowSize.x, 150)))
						{
							for (int i = 0; i < textures.size(); ++i)
							{
								const bool selected = selectedTexture == i;
								if (ImGui::Selectable(textures[i]->getName().c_str(), selected))
									selectedTexture = i;
							}
						}
						ImGui::EndChild();

						ImGui::Text("Name: %s", textures[selectedTexture]->getName().c_str());
						ImGui::Text("Unit: %s", textures[selectedTexture]->getUnit().c_str());
						ImGui::Text("UV Index: %d", textures[selectedTexture]->getUVIndex());
						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
				*/
			}

			ImGui::End();
		}

		void ModelViewer::update(Renderer* renderer, float deltaT)
		{
			if (ImGui::Begin(UI::mViewerWindow, NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
			{
				viewport.cameraControl();
				ImGui::SameLine();
				viewport.lightControl();
				ImGui::SameLine();
				viewport.renderingControl();

				ImGui::SameLine();
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::SameLine();

				if (UI::transparentButton(ICON_FA_STOP, UI::btnNormal))
					stopPlayback();

				ImGui::SameLine();
				if (UI::transparentButton(playing ? ICON_FA_PAUSE : ICON_FA_PLAY, UI::btnNormal))
					togglePlayback();

				ImGui::SameLine();
				if (UI::transparentButton(ICON_FA_REDO_ALT, UI::btnNormal))
					replay();

				static const char* speedsChar[]{ "0.25x", "0.50x", "0.75x", "1.00x" };
				static float speeds[]{ 0.25f, 0.5f, 0.75f, 1.0f };
				int selectedIndex = (playbackSpeed * 4) - 1;

				ImGui::SameLine();
				ImGui::SetNextItemWidth(100);
				if (ImGui::BeginCombo("Playback Speed", speedsChar[selectedIndex]))
				{
					for (int i = 0; i < 4; ++i)
					{
						const bool selected = selectedIndex == i;
						if (ImGui::Selectable(speedsChar[i], selected))
						{
							selectedIndex = i;
							playbackSpeed = speeds[i];
						}

						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::SameLine();
				ImGui::Checkbox("Loop", &loop);

				ImGui::SameLine();
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::SameLine();

				ImGui::Checkbox("Grid", &drawGrid);

				updatePreview(renderer, deltaT);
			}
			ImGui::End();

			modelProperties();
		}

	}
}

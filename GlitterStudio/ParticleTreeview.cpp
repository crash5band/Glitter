#include "ParticleEditor.h"
#include "GTMManager.h"
#include "ImGui/imgui.h"
#include "UI.h"

namespace Glitter
{
	namespace Editor
	{
		static ImGuiTreeNodeFlags parentNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;
		static ImGuiTreeNodeFlags selectedParentFlags = parentNodeFlags | ImGuiTreeNodeFlags_Selected;
		static ImGuiTreeNodeFlags childNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;
		static ImGuiTreeNodeFlags selectedChildFlags = childNodeFlags | ImGuiTreeNodeFlags_Selected;
		ImGuiTreeNodeFlags nodeFlags;

		bool ParticleEditor::effectMenu(int index)
		{
			bool closed = false;
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Add Emitter"))
					createEmitter(effects[index], "emitter");

				if (ImGui::MenuItem("Add Particle"))
					createParticle(effects[index], "particle");

				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_PASTE " Paste"))
				{
					selectedEffect = index;
					paste();
				}

				ImGui::Separator();

				ImGui::Separator();
				if (ImGui::MenuItem("Save"))
					save(index, false);

				if (ImGui::MenuItem("Save As..."))
					save(index, true);

				ImGui::Separator();
				if (ImGui::MenuItem("Close"))
				{
					closeEffect(index);
					if (selectedEffect == index)
					{
						selectedEffect = -1;
						selectedChild = -1;
					}

					closed = true;
				}

				ImGui::EndPopup();
			}

			return closed;
		}

		bool ParticleEditor::emitterMenu(int parent, int index)
		{
			if (ImGui::BeginPopupContextItem())
			{
				std::shared_ptr<EmitterNode> em = effects[parent]->getEmitterNodes().at(index);
				if (ImGui::BeginMenu("Add Particle"))
				{
					std::vector<std::weak_ptr<ParticleNode>> availableParticles = getAvailableParticles(effects[parent], em);
					const size_t count = availableParticles.size();

					ImGui::Text("Available Particles");
					ImGui::Separator();
					if (count)
					{
						for (size_t i = 0; i < count; ++i)
						{
							if (ImGui::MenuItem(availableParticles[i].lock()->getParticle()->getName().c_str()))
								addParticleInstance(em, availableParticles[i].lock(), em->getParticles().size());
						}
					}
					else
					{
						ImGui::MenuItem("None Available");
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_COPY " Copy"))
				{
					nodeCopy = em;
				}

				if (ImGui::MenuItem(ICON_FA_PASTE " Paste"))
				{
					selectedEffect = parent;
					paste();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Remove", "Del"))
				{
					removeEmitter(effects[parent], index);
					ImGui::EndPopup();
					return true;
				}

				if (ImGui::MenuItem("Hide All"))
					effects[parent]->getEmitterNodes()[index]->setVisibleAll(false);

				if (ImGui::MenuItem("Show All"))
					effects[parent]->getEmitterNodes()[index]->setVisibleAll(true);

				ImGui::EndPopup();
			}

			return false;
		}

		bool ParticleEditor::particleMenu(int parent, int index)
		{
			bool removed = false;
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem(ICON_FA_COPY " Copy"))
				{
					nodeCopy = effects[parent]->getParticleNodes().at(index);
				}

				if (ImGui::MenuItem(ICON_FA_PASTE " Paste"))
				{
					selectedEffect = parent;
					paste();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Remove", "Del"))
				{
					removeParticle(effects[parent], index);
					removed = true;
				}

				ImGui::EndPopup();
				return removed;
			}
		}

		bool ParticleEditor::instanceMenu(int effect, int emitter, int index)
		{
			bool removed = false;
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Select Particle"))
				{
					std::shared_ptr<ParticleNode> node = effects[effect]->getEmitterNodes().at(emitter)->getParticles().at(index).getReference();
					std::vector<std::shared_ptr<ParticleNode>> particles = effects[effect]->getParticleNodes();
					for (int p = 0; p < particles.size(); ++p)
					{
						if (particles[p] == node)
						{
							selectedEffect = effect;
							selectedChild = effects[effect]->getEmitterNodes().size() + p;
							break;
						}
					}
				}

				if (ImGui::MenuItem("Remove"))
				{
					std::shared_ptr<EmitterNode> node = effects[effect]->getEmitterNodes()[emitter];
					removeParticleInstance(node, node->getParticles()[index].getReference(), index);
					removed = true;
				}

				ImGui::EndPopup();
				return removed;
			}
		}

		bool ParticleEditor::materialMenu(int index)
		{
			bool closed = false;
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Save"))
					GTMManager::save(index, false);

				if (ImGui::MenuItem("Save As..."))
					GTMManager::save(index, true);

				ImGui::Separator();
				if (ImGui::MenuItem("Duplicate"))
					GTMManager::duplicate(index);

				ImGui::Separator();
				if (ImGui::MenuItem("Close"))
				{
					GTMManager::remove(index);
					closed = true;
				}

				ImGui::EndPopup();
			}

			return closed;
		}

		void ParticleEditor::setSelectedNode()
		{
			std::shared_ptr<INode> node = nullptr;
			if (selectedEffect >= 0 && selectedEffect < effects.size())
			{
				int emitterCount = effects[selectedEffect]->getEmitterNodes().size();
				int particleCount = effects[selectedEffect]->getParticleNodes().size();

				if (selectedChild == -1)
					node = effects[selectedEffect];
				else if (selectedChild >= 0 && selectedChild < emitterCount)
					node = effects[selectedEffect]->getEmitterNodes().at(selectedChild);
				else if (selectedChild >= emitterCount && selectedChild < emitterCount + particleCount)
					node = effects[selectedEffect]->getParticleNodes().at(selectedChild - emitterCount);

				animationTimeline.setAnimations(node->getAnimationSet());
				player.setEffect(effects[selectedEffect].get());
			}

			if (selectionMode == SelectionMode::GTM)
			{
				if (selectedMaterial >= 0 && selectedMaterial < GTMManager::count())
					inspector.setNode(GTMManager::getNodes().at(selectedMaterial));
			}
			else
			{
				inspector.setNode(node);
			}
		}

		void ParticleEditor::updateTreeview()
		{
			ImGui::Begin(UI::effWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus);
			UI::itemRowsBackground();

			for (size_t i = 0; i < effects.size(); ++i)
			{
				std::string effectName = effects[i]->getEffect()->getName();
				nodeFlags = UI::isNodeSelected(selectedEffect, i, selectedChild, -1) ? selectedParentFlags : parentNodeFlags;
				bool open = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "");

				if (effectMenu(i))
				{
					--i;
					if (open) ImGui::TreePop();

					continue;
				}
				
				if (UI::selectNode(selectedEffect, i, selectedChild, -1))
					selectionMode = SelectionMode::GTE;

				ImGui::SameLine();
				ImGui::Text("%s %s", open ? ICON_FA_FOLDER_OPEN : ICON_FA_FOLDER, effectName.c_str());

				if (open)
				{
					// Emitters
					float btnSize = ImGui::GetFrameHeightWithSpacing();

					std::vector<std::shared_ptr<EmitterNode>>& emitterNodes = effects[i]->getEmitterNodes();
					for (int j = 0; j < emitterNodes.size(); ++j)
					{
						ImVec2 cursorPos = ImGui::GetCursorScreenPos();
						cursorPos.x -= btnSize * 0.75f;

						nodeFlags = UI::isNodeSelected(selectedEffect, i, selectedChild, j) ? selectedParentFlags : parentNodeFlags;
						bool emOpen = ImGui::TreeNodeEx((void*)(intptr_t)(i + j), nodeFlags, "%s %s", ICON_FA_CUBE, emitterNodes[j]->getEmitter()->getName().c_str());
						ImVec2 newCursorPos = ImGui::GetCursorPos();

						if (emitterMenu(i, j))
						{
							--j;
							if (emOpen) ImGui::TreePop();

							continue;
						}

						if (UI::selectNode(selectedEffect, i, selectedChild, j))
							selectionMode = SelectionMode::GTE;

						std::string lbl(emitterNodes[j]->isVisible() ? ICON_FA_EYE : ICON_FA_EYE_SLASH);
						lbl.append("##eff_" + std::to_string(i) + "em_" + std::to_string(j));
						ImGui::SetCursorScreenPos(cursorPos);

						if (UI::transparentButton(lbl, ImVec2(btnSize, btnSize)))
							emitterNodes[j]->setVisible(!emitterNodes[j]->isVisible());

						ImGui::SetCursorPos(newCursorPos);

						if (emOpen)
						{
							// Emitter particles
							if (j < emitterNodes.size())
							{
								std::vector<ParticleInstance>& emitterParticles = emitterNodes[j]->getParticles();
								for (int p = 0; p < emitterParticles.size(); ++p)
								{
									ImVec2 cursorPos = ImGui::GetCursorScreenPos();
									ImGui::TreeNodeEx((void*)(intptr_t)(i + 1 + ((j + 1) * p)), childNodeFlags, emitterParticles[p].getParticle()->getName().c_str());
									ImVec2 newCursorPos = ImGui::GetCursorPos();

									if (instanceMenu(i, j, p))
									{
										--p;
										continue;
									}

									std::string lbl(emitterParticles[p].isVisible() ? ICON_FA_EYE : ICON_FA_EYE_SLASH);
									lbl.append("##em_" + std::to_string(j) + "p_" + std::to_string(p));
									ImGui::SetCursorScreenPos(cursorPos);

									if (UI::transparentButton(lbl, ImVec2(btnSize, btnSize)))
										emitterParticles[p].setVisible(!emitterParticles[p].isVisible());

									ImGui::SetCursorPos(newCursorPos);
								}
							}

							ImGui::TreePop();
						}
					}

					// Particles
					std::vector<std::shared_ptr<ParticleNode>>& particleNodes = effects[i]->getParticleNodes();
					for (int k = 0; k < particleNodes.size(); ++k)
					{
						nodeFlags = UI::isNodeSelected(selectedEffect, i, selectedChild, emitterNodes.size() + k) ? selectedChildFlags : childNodeFlags;
						ImGui::TreeNodeEx((void*)(intptr_t)(i + 1 + k + emitterNodes.size()), nodeFlags, "%s %s", ICON_FA_CERTIFICATE, particleNodes[k]->getParticle()->getName().c_str());

						if (particleMenu(i, k))
						{
							selectedChild = -1;
						}

						if (UI::selectNode(selectedEffect, i, selectedChild, emitterNodes.size() + k))
							selectionMode = SelectionMode::GTE;
					}

					ImGui::TreePop();
				}
			}

			ImGui::End();

			ImGui::Begin(ICON_FA_LAYER_GROUP " Materials");
			UI::itemRowsBackground();

			std::vector<std::shared_ptr<MaterialNode>> materials = GTMManager::getNodes();
			for (int m = 0; m < materials.size(); ++m)
			{
				nodeFlags = selectedMaterial == m ? selectedChildFlags : childNodeFlags;
				ImGui::TreeNodeEx((void*)(intptr_t)m, nodeFlags, "%s %s", ICON_FA_SQUARE, materials[m]->getMaterial()->getName().c_str());
				if (ImGui::IsItemActivated())
				{
					selectedMaterial = m;
					selectionMode = SelectionMode::GTM;
				}

				materialMenu(m);

				if (ImGui::IsItemHovered() && materials[m]->getTexture())
				{
					ImGui::BeginTooltipEx(0, 0);
					inspector.previewTexture(materials[m]->getTexture(), materials[m]->getMaterial()->getSplit(), false, false);
					ImGui::EndTooltip();
				}
			}

			ImGui::End();
			setSelectedNode();
		}
	}
}
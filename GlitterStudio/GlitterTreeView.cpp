#include "Editor.h"
#include "UiHelper.h"
#include "File.h"
#include "FileGUI.h"
#include "IconsFontAwesome5.h"
#include "Logger.h"

static ImGuiTreeNodeFlags parentNodeFlags		= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;
static ImGuiTreeNodeFlags selectedParentFlags	= parentNodeFlags | ImGuiTreeNodeFlags_Selected;
static ImGuiTreeNodeFlags childNodeFlags		= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;
static ImGuiTreeNodeFlags selectedChildFlags	= childNodeFlags | ImGuiTreeNodeFlags_Selected;
ImGuiTreeNodeFlags nodeFlags;

void Editor::updateMenuBar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem(ICON_FA_FILE " Open", "Ctrl + O"))
		{
			std::string name;
			if (FileGUI::openFileGUI(FileType::Effect, name))
				openGlitterFile(name);
		}

		ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open Folder (Dummy)");
		ImGui::Separator();

		if (ImGui::MenuItem(ICON_FA_SAVE " Save", "Ctrl + S"))
			saveEffect(false);

		if (ImGui::MenuItem(ICON_FA_SAVE " Save As", " Ctrl + Alt + S"))
			saveEffect(true);

		ImGui::Separator();
		if (ImGui::MenuItem(ICON_FA_TIMES " Quit"))
			glfwSetWindowShouldClose(window, 1);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem(ICON_FA_UNDO " Undo", "Ctrl + Z"))
			CommandManager::undo();

		if (ImGui::MenuItem(ICON_FA_REDO " Redo", "Ctrl + Y"))
			CommandManager::redo();

		ImGui::Separator();
		if (ImGui::MenuItem("New Effect"))
			createEffect("effect");

		if (ImGui::MenuItem("New Emitter"))
		{
			if (selectedParent > -1 && selectedParent < effectNodes.size())
			{
				createEmitter(effectNodes[selectedParent], "emitter");
			}
		}

		if (ImGui::MenuItem("New Particle"))
		{
			if (selectedParent > -1 && selectedParent < effectNodes.size())
			{
				createParticle(effectNodes[selectedParent], "particle");
			}
		}

		ImGui::Separator();
		if (ImGui::MenuItem("New Material"))
			createMaterial("material");

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem("History View", NULL, &Editor::debugWindows.historyViewOpen);
		ImGui::MenuItem("Active Particles", NULL, &Editor::debugWindows.particlesOpen);

		if (ImGui::MenuItem("Clear Log"))
			Logger::clear();

		ImGui::Separator();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Materials"))
	{
		if (ImGui::MenuItem(ICON_FA_FILE " Open"))
		{
			std::string name;
			if (FileGUI::openFileGUI(FileType::Material, name))
				openGlitterFile(name);
		}

		ImGui::Separator();
		if (ImGui::MenuItem(ICON_FA_SAVE " Save"))
			saveMaterial(false);

		if (ImGui::MenuItem(ICON_FA_SAVE " Save As"))
			saveMaterial(true);

		ImGui::EndMenu();
	}

#ifdef _DEBUG
	if (ImGui::BeginMenu("Debug"))
	{
		ImGui::MenuItem("Debug Info", NULL, &Editor::debugWindows.performanceOpen);

		ImGui::Separator();
		if (ImGui::MenuItem("Reset Editor", NULL))
			reset();

		ImGui::EndMenu();
	}
#endif

	if (ImGui::BeginMenu("Preferences"))
	{
		if (ImGui::MenuItem("VSync", NULL, Editor::editorSettings.vsync))
			glfwSwapInterval(Editor::editorSettings.vsync ^= true);

		if (ImGui::MenuItem("Hue Wheel", NULL, Editor::editorSettings.colorWheel))
			Editor::editorSettings.colorWheel ^= true;

		ImGui::EndMenu();
	}

	bool open = false;
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("About", NULL))
			open = true;

		ImGui::EndMenu();
	}

	ImGui::PopStyleVar(1);
	ImGui::EndMainMenuBar();

	if (open)
		ImGui::OpenPopup("About");

	about();
}

bool Editor::isNodeSelected(int parentIndex, int childIndex)
{
	return selectedParent == parentIndex && selectedChild == childIndex;
}

bool Editor::effectMenu(int index)
{
	bool closed = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Selectable(ICON_FA_CUBE " Add Emitter"))
			createEmitter(effectNodes[index], "emitter");

		if (ImGui::Selectable(ICON_FA_CERTIFICATE " Add Particle"))
			createParticle(effectNodes[index], "particle");

		if (ImGui::Selectable(ICON_FA_TIMES " Close"))
		{
			closeEffect(index);
			selectedChild = selectedParent == index ? selectedChild : -1;
			if (selectedParent == index)
			{
				selectedParent = -1;
				selectedChild = -1;
			}

			closed = true;
		}

		ImGui::EndPopup();
	}

	return closed;
}

bool Editor::emitterMenu(int parent, int index)
{
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Selectable(ICON_FA_CERTIFICATE " Add Particle"))
		{
			showAvailableParticles = true;
			showEffect = parent;
			showEmitter = index;
		}

		ImGui::Separator();
		std::string str = ICON_FA_TRASH_ALT " Remove##" + std::to_string(parent + index);
		if (ImGui::Selectable(str.c_str()))
		{
			removeEmitter(effectNodes[parent], index);
			ImGui::EndPopup();
			return true;
		}

		std::string hStr = "Hide All##" + std::to_string(parent + index);
		std::string sStr = "Show All##" + std::to_string(parent + index);

		if (ImGui::Selectable(hStr.c_str()))
			effectNodes[parent]->getEmitterNodes()[index]->setVisibleAll(false);

		if (ImGui::Selectable(sStr.c_str()))
			effectNodes[parent]->getEmitterNodes()[index]->setVisibleAll(true);

		ImGui::EndPopup();
		return false;
	}
}

bool Editor::particleMenu(int parent, int index)
{
	bool removed = false;
	if (ImGui::BeginPopupContextItem())
	{
		std::string str = ICON_FA_TRASH_ALT " Remove##" + std::to_string(parent + index);
		if (ImGui::Selectable(str.c_str()))
		{
			removeParticle(effectNodes[parent], index);
			removed = true;
		}

		ImGui::EndPopup();
		return removed;
	}
}

bool Editor::instanceMenu(int effect, int parent, int index)
{
	bool removed = false;
	if (ImGui::BeginPopupContextItem())
	{
		std::string str = ICON_FA_TRASH_ALT " Remove##" + std::to_string(effect + parent + index);
		if (ImGui::Selectable(str.c_str()))
		{
			std::shared_ptr<EmitterNode> node = effectNodes[effect]->getEmitterNodes()[parent];
			removeParticleInstance(node, node->getParticles()[index].getReference(), index);
			removed = true;
		}

		ImGui::EndPopup();
		return removed;
	}
}

void Editor::availableParticlesMenu(int parent, int index)
{
	if (showAvailableParticles)
	{
		ImGui::OpenPopup("Available Particles##popup");
		availableParticles = getAvailableParticles(effectNodes[parent], effectNodes[parent]->getEmitterNodes()[index]);
		showAvailableParticles = false;
	}

	if (!effectNodes[parent]->getEmitterNodes().size() || index >= effectNodes[parent]->getEmitterNodes().size())
		return;

	std::shared_ptr<EmitterNode>& em = effectNodes[parent]->getEmitterNodes()[index];

	if (ImGui::BeginPopup("Available Particles##popup"))
	{
		const size_t count = availableParticles.size();
		ImGui::Text("Available Particles");
		if (count)
		{
			for (size_t i = 0; i < count; ++i)
			{
				std::string lbl(ICON_FA_CERTIFICATE " " + availableParticles[i].lock()->getName() + "##" + std::to_string(i));
				if (ImGui::Selectable(lbl.c_str()))
				{
					for (int j = 0; j < effectNodes[parent]->getParticleNodes().size(); ++j)
					{
						std::shared_ptr<ParticleNode> pNode = effectNodes[parent]->getParticleNodes()[j];
						if (pNode->getParticle() == availableParticles[i].lock())
							addParticleInstance(em, pNode, em->getParticles().size());
					}
				}
			}
		}
		else
		{
			ImGui::Selectable("None Available");
		}

		ImGui::EndPopup();
	}
}

void Editor::setNodeSelected(int p, int c)
{
	if (ImGui::IsItemActivated())
	{
		selectedParent = p;
		selectedChild = c;
	}
}

void Editor::setSelectedNode()
{
	std::weak_ptr<IGlitterNode> node;
	EffectNode* eff = nullptr;
	if (selectedParent > -1)
	{
		size_t emitterSize = effectNodes[selectedParent]->getEmitterNodes().size();
		size_t particleSize = effectNodes[selectedParent]->getParticleNodes().size();

		if (selectedChild < emitterSize && selectedChild > -1)
			node = effectNodes[selectedParent]->getEmitterNodes()[selectedChild];
		else if (selectedChild >= emitterSize && selectedChild < emitterSize + particleSize)
			node = effectNodes[selectedParent]->getParticleNodes()[selectedChild - emitterSize];
		else
			node = effectNodes[selectedParent];

		eff = effectNodes[selectedParent].get();
	}

	inspector->setNode(node.lock());
	player->setEffect(eff);
}

void Editor::updateGlitterTreeView()
{
	ImGui::Begin(effWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus);
	itemRowsBackground();

	for (size_t i = 0; i < effectNodes.size(); ++i)
	{
		std::string effectName = effectNodes[i]->getEffect()->getName();
		nodeFlags = isNodeSelected(i, -1) ? selectedParentFlags : parentNodeFlags;
		bool open = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "");
		if (effectMenu(i))
		{
			--i;
			if (open) ImGui::TreePop();

			continue;
		}
		setNodeSelected(i, -1);

		ImGui::SameLine();
		ImGui::Text("%s %s", open ? ICON_FA_FOLDER_OPEN : ICON_FA_FOLDER, effectName.c_str());

		if (open)
		{
			// Emitters
			std::vector<std::shared_ptr<EmitterNode>>& emitterNodes = effectNodes[i]->getEmitterNodes();
			size_t emitterCount = emitterNodes.size();

			for (size_t j = 0; j < emitterNodes.size(); ++j)
			{
				ImVec2 cursorPos = ImGui::GetCursorScreenPos();
				cursorPos.x -= btnSmall.x;
				nodeFlags = isNodeSelected(i, j) ? selectedParentFlags : parentNodeFlags;
				bool emOpen = ImGui::TreeNodeEx((void*)(intptr_t)(i + j), nodeFlags, "%s %s", ICON_FA_CUBE, emitterNodes[j]->getEmitter()->getName().c_str());
				
				/*
					Returns true if an emitter is removed. In this case, we skip the current iteration,
					subtract the emitter index by one (to counter the increase from skipping the iteration.
					
					Also need to make sure to call ImGui::TreePop() if the removed emitter node was open
				*/ 
				if (emitterMenu(i, j))
				{
					--j;
					if (emOpen) ImGui::TreePop();

					continue;
				}
				
				if (i == showEffect && j == showEmitter)
					availableParticlesMenu(i, j);

				setNodeSelected(i, j);

				std::string lbl(emitterNodes[j]->isVisible() ? ICON_FA_EYE : ICON_FA_EYE_SLASH);
				lbl.append("##" + std::to_string(i + j));
				ImGui::SetCursorScreenPos(cursorPos);

				if (transparentButton(lbl, ImVec2(24, 24)))
					emitterNodes[j]->setVisible(!emitterNodes[j]->isVisible());
					
				if (emOpen)
				{
					// Emitter particles
					if (j < emitterNodes.size())
					{
						std::vector<ParticleInstance>& emitterParticles = emitterNodes[j]->getParticles();
						for (size_t p = 0; p < emitterParticles.size(); ++p)
						{
							ImVec2 cursorPos = ImGui::GetCursorScreenPos();
							ImGui::TreeNodeEx((void*)(intptr_t)(i + (j + p + 1) * 100), childNodeFlags, "%s %s", ICON_FA_CIRCLE, emitterParticles[p].getParticle()->getName().c_str());
							
							if (instanceMenu(i, j, p))
							{
								--p;
								continue;
							}

							std::string lbl(emitterParticles[p].isVisible() ? ICON_FA_EYE : ICON_FA_EYE_SLASH);
							lbl.append("##" + std::to_string(i + (j + p + 1) * 100));
							ImGui::SetCursorScreenPos(cursorPos);

							if (transparentButton(lbl, ImVec2(24, 24)))
								emitterParticles[p].setVisible(!emitterParticles[p].isVisible());
								
						}
					}

					ImGui::TreePop();
				}
			}

			// Particles
			std::vector<std::shared_ptr<ParticleNode>>& particleNodes = effectNodes[i]->getParticleNodes();
			for (size_t k = 0; k < particleNodes.size(); ++k)
			{
				nodeFlags = isNodeSelected(i, emitterCount + k) ? selectedChildFlags : childNodeFlags;
				ImGui::TreeNodeEx((void*)(intptr_t)(i + k + emitterNodes.size()), nodeFlags, "%s %s", ICON_FA_CERTIFICATE, particleNodes[k]->getParticle()->getName().c_str());
				
				if (particleMenu(i, k))
				{
					--k;
					continue;
				}

				setNodeSelected(i, emitterCount + k);
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
	updateMenuBar();
	setSelectedNode();
}

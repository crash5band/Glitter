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

		if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open Folder"))
		{
			std::string dir;
			if (FileGUI::openFolderGUI(dir))
				openFolder(dir);
		}

		ImGui::Separator();
		if (ImGui::MenuItem(ICON_FA_SAVE " Save", "Ctrl + S"))
			saveEffect(selectedParent, false);

		if (ImGui::MenuItem(ICON_FA_SAVE " Save As", " Ctrl + Alt + S"))
			saveEffect(selectedParent, true);

		if (ImGui::MenuItem(ICON_FA_SAVE " Save All"))
		{
			for (int e = 0; e < effectNodes.size(); ++e)
				saveEffect(e, false);
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Close All Effects"))
			closeAllEffects();

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
		ImGui::MenuItem("History View", NULL, &editorSettings.historyViewOpen);
		ImGui::MenuItem("Active Particles", NULL, &editorSettings.particlesOpen);
		ImGui::MenuItem("Stats Window", NULL, &editorSettings.statsOpen);

		if (ImGui::MenuItem("Clear Log"))
			Logger::clear();

		ImGui::Separator();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Materials"))
	{
		if (ImGui::MenuItem(ICON_FA_FILE " Open", "Alt + O"))
		{
			std::string name;
			if (FileGUI::openFileGUI(FileType::Material, name))
				openGlitterFile(name);
		}

		ImGui::Separator();
		if (ImGui::MenuItem(ICON_FA_SAVE " Save", "Ctrl + Shift + S"))
			saveMaterial(MaterialEditor::getSelection(), false);

		if (ImGui::MenuItem(ICON_FA_SAVE " Save As"))
			saveMaterial(MaterialEditor::getSelection(), true);

		if (ImGui::MenuItem(ICON_FA_SAVE " Save All"))
		{
			for (int m = 0; m < MaterialEditor::count(); ++m)
				saveMaterial(m, false);
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Close All Materials"))
			MaterialEditor::clean();

		ImGui::EndMenu();
	}

#ifdef _DEBUG
	if (ImGui::BeginMenu("Debug"))
	{
		ImGui::MenuItem("Show ImGui Demo Window", NULL, &editorSettings.imguiDemoOpen);

		ImGui::Separator();
		if (ImGui::MenuItem("Reset Editor", NULL))
			reset();

		ImGui::EndMenu();
	}
#endif

	if (ImGui::BeginMenu("Preferences"))
	{
		if (ImGui::MenuItem("VSync", NULL, editorSettings.vsync))
			glfwSwapInterval(editorSettings.vsync ^= true);

		ImGui::MenuItem("Hue Wheel", NULL, &editorSettings.colorWheel);
		ImGui::MenuItem("FPS Meter", NULL, &editorSettings.fpsCounter);
		ImGui::MenuItem("Hover Over Materials To Preview", NULL, &editorSettings.matPreview);

		ImGui::EndMenu();
	}

	bool open = false;
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("About", NULL))
			open = true;

		ImGui::EndMenu();
	}

	if (Editor::editorSettings.fpsCounter)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::Text("%.3f ms (%.3f FPS)", frameDelta * 1000.0f, 1.0f / frameDelta);
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
		if (ImGui::MenuItem(ICON_FA_CUBE " Add Emitter"))
			createEmitter(effectNodes[index], "emitter");

		if (ImGui::MenuItem(ICON_FA_CERTIFICATE " Add Particle"))
			createParticle(effectNodes[index], "particle");

		ImGui::Separator();
		ImGui::Separator();
		if (ImGui::MenuItem(ICON_FA_SAVE " Save"))
			saveEffect(index, false);

		if (ImGui::MenuItem(ICON_FA_SAVE " Save As"))
			saveEffect(index, true);

		ImGui::Separator();
		ImGui::Separator();
		if (ImGui::MenuItem(ICON_FA_TIMES " Close"))
		{
			closeEffect(index);
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
		if (ImGui::BeginMenu(ICON_FA_CERTIFICATE " Add Particle"))
		{
			std::shared_ptr<EmitterNode> em = effectNodes[parent]->getEmitterNodes().at(index);

			availableParticles = getAvailableParticles(effectNodes[parent], em);
			const size_t count = availableParticles.size();

			ImGui::Text("Available Particles");
			if (count)
			{
				for (size_t i = 0; i < count; ++i)
				{
					std::string pName(ICON_FA_CERTIFICATE " " + availableParticles[i].lock()->getParticle()->getName());
					if (ImGui::MenuItem(pName.c_str()))
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
		if (ImGui::MenuItem(ICON_FA_TRASH_ALT " Remove", "Del"))
		{
			removeEmitter(effectNodes[parent], index);
			ImGui::EndPopup();
			return true;
		}

		if (ImGui::MenuItem(ICON_FA_EYE_SLASH " Hide All"))
			effectNodes[parent]->getEmitterNodes()[index]->setVisibleAll(false);

		if (ImGui::MenuItem(ICON_FA_EYE " Show All"))
			effectNodes[parent]->getEmitterNodes()[index]->setVisibleAll(true);

		ImGui::EndPopup();
	}

	return false;
}

bool Editor::particleMenu(int parent, int index)
{
	bool removed = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem(ICON_FA_TRASH_ALT " Remove", "Del"))
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
		if (ImGui::MenuItem(ICON_FA_CERTIFICATE " Select Particle"))
		{
			std::shared_ptr<ParticleNode> node = effectNodes[effect]->getEmitterNodes().at(parent)->getParticles().at(index).getReference();
			std::vector<std::shared_ptr<ParticleNode>> particles = effectNodes[effect]->getParticleNodes();
			for (int p = 0; p < particles.size(); ++p)
			{
				if (particles[p] == node)
				{
					selectedParent	= effect;
					selectedChild	= effectNodes[effect]->getEmitterNodes().size() + p;
					break;
				}
			}
		}

		if (ImGui::MenuItem(ICON_FA_TRASH_ALT " Remove"))
		{
			std::shared_ptr<EmitterNode> node = effectNodes[effect]->getEmitterNodes()[parent];
			removeParticleInstance(node, node->getParticles()[index].getReference(), index);
			removed = true;
		}

		ImGui::EndPopup();
		return removed;
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
			float btnSize = ImGui::GetFrameHeightWithSpacing();

			std::vector<std::shared_ptr<EmitterNode>>& emitterNodes = effectNodes[i]->getEmitterNodes();
			for (int j = 0; j < emitterNodes.size(); ++j)
			{
				ImVec2 cursorPos = ImGui::GetCursorScreenPos();
				cursorPos.x -= btnSize * 0.75f;

				nodeFlags = isNodeSelected(i, j) ? selectedParentFlags : parentNodeFlags;
				bool emOpen = ImGui::TreeNodeEx((void*)(intptr_t)(i + j), nodeFlags, "%s %s", ICON_FA_CUBE, emitterNodes[j]->getEmitter()->getName().c_str());
				ImVec2 newCursorPos = ImGui::GetCursorPos();

				if (emitterMenu(i, j))
				{
					--j;
					if (emOpen) ImGui::TreePop();

					continue;
				}
				
				setNodeSelected(i, j);

				std::string lbl(emitterNodes[j]->isVisible() ? ICON_FA_EYE : ICON_FA_EYE_SLASH);
				lbl.append("##eff_" + std::to_string(i) + "em_" + std::to_string(j));
				ImGui::SetCursorScreenPos(cursorPos);

				if (transparentButton(lbl, ImVec2(btnSize, btnSize)))
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
							ImGui::TreeNodeEx((void*)(intptr_t)(i + 1 + ((j + 1) * p)), childNodeFlags, "%s %s", ICON_FA_CIRCLE, emitterParticles[p].getParticle()->getName().c_str());
							ImVec2 newCursorPos = ImGui::GetCursorPos();

							if (instanceMenu(i, j, p))
							{
								--p;
								continue;
							}

							std::string lbl(emitterParticles[p].isVisible() ? ICON_FA_EYE : ICON_FA_EYE_SLASH);
							lbl.append("##em_" + std::to_string(j) + "p_" + std::to_string(p));
							ImGui::SetCursorScreenPos(cursorPos);

							if (transparentButton(lbl, ImVec2(btnSize, btnSize)))
								emitterParticles[p].setVisible(!emitterParticles[p].isVisible());

							ImGui::SetCursorPos(newCursorPos);
						}
					}

					ImGui::TreePop();
				}
			}

			// Particles
			std::vector<std::shared_ptr<ParticleNode>>& particleNodes = effectNodes[i]->getParticleNodes();
			for (int k = 0; k < particleNodes.size(); ++k)
			{
				nodeFlags = isNodeSelected(i, emitterNodes.size() + k) ? selectedChildFlags : childNodeFlags;
				ImGui::TreeNodeEx((void*)(intptr_t)(i + 1 + k + emitterNodes.size()), nodeFlags, "%s %s", ICON_FA_CERTIFICATE, particleNodes[k]->getParticle()->getName().c_str());
				
				particleMenu(i, k);
				setNodeSelected(i, emitterNodes.size() + k);
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
	updateMenuBar();
	setSelectedNode();
}

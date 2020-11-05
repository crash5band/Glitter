#include "Editor.h"
#include "UiHelper.h"
#include "ResourceManager.h"

void Editor::frameTime()
{
	float currentFrame = glfwGetTime();
	frameDelta = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void Editor::debugInfo()
{
	if (Editor::debugWindows.performanceOpen)
	{
		if (ImGui::Begin(debugWindow, &Editor::debugWindows.performanceOpen, ImGuiWindowFlags_NoBringToFrontOnFocus))
		{
			if (ImGui::TreeNodeEx("I/O", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Mouse: %.1f, %.1f", io->MousePos.x, io->MousePos.y);
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Resources", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Models: %d", ResourceManager::getModelCount());
				ImGui::Text("Textures: %d", ResourceManager::getTextureCount());
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Performance", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("%.3f ms (%.3f FPS)", frameDelta * 1000.0f, 1.0f / frameDelta);
				ImGui::Separator();

				for (size_t i = 0; i < times.size(); ++i)
					ImGui::Text("%s: %.3f ms", times[i].name, times[i].time * 1000);

				ImGui::TreePop();
			}
		}

		ImGui::End();
	}

	ImGui::BeginMainMenuBar();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::Text("%.3f ms (%.3f FPS)", frameDelta * 1000.0f, 1.0f / frameDelta);
	ImGui::EndMainMenuBar();

	if (Editor::debugWindows.historyViewOpen)
	{
		if (ImGui::Begin(historyWindow, &Editor::debugWindows.historyViewOpen))
		{
			ImGui::Columns(2, "##history_cols", false);
			ImGui::SetColumnOffset(1, 0);
			ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x / 2.0f);

			std::stack<ICommand*> undo = CommandManager::getUndoHistory();
			std::stack<ICommand*> redo = CommandManager::getRedoHistory();

			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::Text("Undo (%d)", undo.size());
			addStackView("##undo", undo);

			ImGui::NextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::Text("Redo (%d)", redo.size());
			addStackView("##redo", redo);

			ImGui::EndColumns();
			ImGui::Separator();
			
			if (ImGui::Button("Clear All", ImVec2(ImGui::GetContentRegionAvail().x, 30)))
				CommandManager::clearAll();
		}

		ImGui::End();
	}

	if (Editor::debugWindows.particlesOpen)
	{
		if (ImGui::Begin("Active Particles", &Editor::debugWindows.particlesOpen))
		{
			EffectNode* selectedEffect = nullptr;
			if (selectedParent > -1 && selectedParent < effectNodes.size())
				selectedEffect = effectNodes[selectedParent].get();
			
			if (!selectedEffect)
			{
				ImGui::End();
				return;
			}

			for (int i = 0; i < selectedEffect->getEmitterNodes().size(); ++i)
			{
				EmitterNode* emitterNode = selectedEffect->getEmitterNodes()[i].get();
				if (ImGui::TreeNodeEx(emitterNode->getEmitter()->getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					for (int j = 0; j < emitterNode->getParticles().size(); ++j)
					{
						ParticleInstance& pInstance = emitterNode->getParticles()[j];
						if (ImGui::TreeNodeEx(pInstance.getParticle()->getName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::Text("Alive: %d", pInstance.getAliveCount());
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
		}

		ImGui::End();
	}
}
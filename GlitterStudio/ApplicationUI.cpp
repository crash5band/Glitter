#include "Application.h"
#include "ImGui/imgui.h"
#include "IconsFontAwesome5.h"
#include "CommandManager.h"
#include "ResourceManager.h"
#include "FileDialog.h"
#include "UI.h"

namespace Glitter
{
	namespace Editor
	{
		void Application::updateUI()
		{
			if (imguiDemoWindow)
				ImGui::ShowDemoWindow(&imguiDemoWindow);

			UI::initLayout(pDockSpaceID);
			particleEditor->update(renderer, frameDelta);

			debugInfo();
			updateMenubar();
		}

		void Application::updateMenubar()
		{
			ImGui::BeginMainMenuBar();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit", "Alt + F4"))
					glfwSetWindowShouldClose(window, 1);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				std::string undoStr{ ICON_FA_UNDO " Undo " };
				undoStr.append(CommandManager::peekUndo());

				std::string redoStr{ ICON_FA_REDO " Redo " };
				redoStr.append(CommandManager::peekRedo());

				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_UNDO " Undo", "Ctrl + Z"))
					CommandManager::undo();

				if (ImGui::MenuItem(ICON_FA_REDO " Redo", "Ctrl + Y"))
					CommandManager::redo();

				ImGui::EndMenu();
			}

			// editors fill this menu
			if (ImGui::BeginMenu("View"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("VSync", NULL, &vsync))
					glfwSwapInterval((int)vsync);

				ImGui::MenuItem("FPS", NULL, &fpsMeter);

				ImGui::EndMenu();
			}

#ifdef _DEBUG
			if (ImGui::BeginMenu("Debug"))
			{
				ImGui::MenuItem("Show ImGui Demo Window", NULL, &imguiDemoWindow);
				ImGui::MenuItem("FPS Meter", NULL, &fpsMeter);
				ImGui::MenuItem("Debug Info", NULL, &debugView);
				ImGui::EndMenu();
			}
#endif // _DEBUG

			if (ImGui::BeginMenu("About"))
			{
				if (ImGui::MenuItem("About"))
					aboutOpen = true;

				ImGui::EndMenu();
			}

			if (fpsMeter)
			{
				ImGui::Text("%.3f ms (%.1f FPS)", frameDelta * 1000, 1 / frameDelta);
			}

			ImGui::EndMainMenuBar();

			if (aboutOpen)
			{
				ImGui::OpenPopup("About");
				aboutOpen = false;
			}

			about();
		}

		void Application::debugInfo()
		{
			if (!debugView)
				return;

			ImGuiWindow* debugWindow = ImGui::FindWindowByName(ICON_FA_BUG " Debug Info");
			if (debugWindow)
			{
				ImGui::BringWindowToDisplayFront(debugWindow);
			}

			if (ImGui::Begin(ICON_FA_BUG " Debug Info", &debugView))
			{
				ImGuiTreeNodeFlags f = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx("Performance", f | ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Frametime: %.3fms (%.2f FPS)", frameDelta * 1000, 1 / frameDelta);

					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("Resources", f))
				{
					ImGui::Text("Models: %d", ResourceManager::getModelCount());
					ImGui::Text("Textures: %d", ResourceManager::getTextureCount());

					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("Command history", f))
				{
					ImGui::Text("Undo history size: %d", CommandManager::getUndoHistory().size());
					ImGui::Text("Redo history size: %d", CommandManager::getRedoHistory().size());

					ImGui::Separator();
					if (ImGui::Button("Clear Command History", ImVec2(ImGui::GetContentRegionAvail().x - ImGui::GetTreeNodeToLabelSpacing(), UI::btnHeight)))
						CommandManager::clearAll();

					ImGui::TreePop();
				}

			}
			ImGui::End();
		}
	}
}
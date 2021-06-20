#include "Application.h"
#include "ImGui/imgui.h"
#include "IconsFontAwesome5.h"
#include "CommandManager.h"
#include "FileGUI.h"
#include "UI.h"

namespace Glitter
{
	namespace Editor
	{
		int Application::screenWidth;
		int Application::screenHeight;

		void Application::updateUI()
		{

			if (imguiDemoWindow)
				ImGui::ShowDemoWindow(&imguiDemoWindow);

			switch (mode)
			{
			case EditorMode::Particle:
				UI::resizeLayout(pDockSpaceID, screenWidth, screenHeight);
				UI::initParticleLayout(pDockSpaceID, screenWidth, screenHeight);
				particleEditor->update(renderer, frameDelta);
				break;

			case EditorMode::Model:
				UI::resizeLayout(mDockSpaceID, screenWidth, screenHeight);
				UI::initModelLayout(mDockSpaceID, screenWidth, screenHeight);
				modelEditor->update(renderer, frameDelta);
				break;

			default:
				break;
			}

			float size = ImGui::GetStyle().WindowPadding.y + ImGui::GetStyle().FramePadding.y + 5;
			ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight() - 1));
			ImGui::SetNextWindowSize(ImVec2(screenWidth, 10), ImGuiCond_Always);
			ImGui::Begin("Mode Select", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

			if (ImGui::Selectable("Particle Editor", mode == EditorMode::Particle, 0,
				ImVec2(ImGui::GetContentRegionAvail().x / 2.0f, size)))
				mode = EditorMode::Particle;

			// Disable model viewer for now
			ImGui::SameLine();
			if (ImGui::Selectable("Model Viewer", mode == EditorMode::Model, 0,
				ImVec2(ImGui::GetContentRegionAvail().x, size)))
				mode = EditorMode::Particle;

			ImGui::End();
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
				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_UNDO " Undo", "Ctrl + Z"))
					CommandManager::undo();

				if (ImGui::MenuItem(ICON_FA_REDO " Redo", "Ctrl + Y"))
					CommandManager::redo();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("VSync", NULL, &vsync))
					glfwSwapInterval((int)vsync);

				ImGui::EndMenu();
			}

#ifdef _DEBUG
			if (ImGui::BeginMenu("Debug"))
			{
				ImGui::MenuItem("Show ImGui Demo Window", NULL, &imguiDemoWindow);
				ImGui::MenuItem("FPS Meter", NULL, &fpsMeter);
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
	}
}
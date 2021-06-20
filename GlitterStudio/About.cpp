#include "Application.h"
#include "ImGui/imgui.h"
#include "UI.h"
#include <Windows.h>

namespace Glitter
{
	namespace Editor
	{
		void Application::getVersion(char* buffer)
		{
			char filename[1024];
			strcpy_s(filename, std::string(appDir + "GlitterStudio.exe").c_str());

			DWORD  verHandle = 0;
			UINT   size = 0;
			LPBYTE lpBuffer = NULL;
			DWORD  verSize = GetFileVersionInfoSize(filename, &verHandle);

			int major = 0, minor = 0, build = 0, rev = 0;
			if (verSize != NULL)
			{
				LPSTR verData = new char[verSize];

				if (GetFileVersionInfo(filename, verHandle, verSize, verData))
				{
					if (VerQueryValue(verData, "\\", (VOID FAR * FAR*) & lpBuffer, &size))
					{
						if (size)
						{
							VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
							if (verInfo->dwSignature == 0xfeef04bd)
							{
								major = (verInfo->dwFileVersionMS >> 16) & 0xffff;
								minor = (verInfo->dwFileVersionMS >> 0) & 0xffff;
								rev = (verInfo->dwFileVersionLS >> 16) & 0xffff;
							}
						}
					}
				}
				delete[] verData;
			}

			sprintf(buffer, "%d.%d.%d", major, minor, rev);
		}

		void Application::about()
		{
			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

			if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_NoResize))
			{
				ImGui::Text("Glitter Studio\nCopyright (C) 2021 Crash5b\n\n");
				ImGui::Separator();

				char buffer[64];
				getVersion(buffer);

				ImGui::Text("Version %s", buffer);
				ImGui::Text("Github: https://github.com/crash5band/Glitter");

				ImGui::Separator();

				ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - 35.0f));
				if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, UI::btnHeight)))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
		}
	}
}
#include "UI.h"
#include "ImGui/imgui_internal.h"

namespace Glitter
{
	namespace Editor
	{
		const char* UI::effWindow = ICON_FA_MAGIC " Effects";
		const char* UI::matWindow = ICON_FA_LAYER_GROUP " Materials";
		const char* UI::pTimelineWindow = ICON_FA_CLOCK " Animation##particle";
		const char* UI::gPlayerWindow = ICON_FA_IMAGE " Preview";
		const char* UI::inspectorWindow = ICON_FA_INFO_CIRCLE " Inspector";
		const char* UI::modelWindow = ICON_FA_CUBES " Models";
		const char* UI::texWindow = ICON_FA_IMAGES " Textures";
		const char* UI::mViewerWindow = ICON_FA_IMAGE " Model View";
		const char* UI::mPropertiesWindow = ICON_FA_INFO_CIRCLE " Model View Properties";
		const char* UI::mTimelineWindow = ICON_FA_CLOCK " Animation##model";

		float UI::btnHeight = 23.0f;
		ImVec2 UI::btnSmall = ImVec2(btnHeight - 5, btnHeight - 5);
		ImVec2 UI::btnNormal = ImVec2(btnHeight, btnHeight);
		ImVec2 UI::btnWide = ImVec2(50.0f, btnHeight);

		void UI::itemRowsBackground(float lineHeight, const ImColor& color)
		{
			auto* drawList = ImGui::GetWindowDrawList();
			const auto& style = ImGui::GetStyle();

			if (lineHeight < 0)
			{
				lineHeight = ImGui::GetFrameHeightWithSpacing();
			}

			float scrollOffsetH = ImGui::GetScrollX();
			float scrollOffsetV = ImGui::GetScrollY();
			float scrolledOutLines = floorf(scrollOffsetV / lineHeight);
			scrollOffsetV -= lineHeight * scrolledOutLines;

			ImVec2 clipRectMin(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
			ImVec2 clipRectMax(clipRectMin.x + ImGui::GetWindowWidth(), clipRectMin.y + ImGui::GetWindowHeight());

			if (ImGui::GetScrollMaxX() > 0)
			{
				clipRectMax.y -= style.ScrollbarSize;
			}

			drawList->PushClipRect(clipRectMin, clipRectMax);

			bool isOdd = (static_cast<int>(scrolledOutLines) & 1);

			float yMin = clipRectMin.y - scrollOffsetV + ImGui::GetCursorPosY();
			float yMax = clipRectMax.y - scrollOffsetV + lineHeight;
			float xMin = clipRectMin.x + scrollOffsetH + ImGui::GetWindowContentRegionMin().x;
			float xMax = clipRectMin.x + scrollOffsetH + ImGui::GetWindowContentRegionMax().x;

			for (float y = yMin; y < yMax; y += lineHeight, isOdd ^= true)
			{
				if (isOdd)
					drawList->AddRectFilled({ xMin, y - style.ItemSpacing.y + 2 }, { xMax, y + lineHeight - 2 }, color);
			}

			drawList->PopClipRect();
		}

		void UI::resizeLayout(ImGuiID dockspaceID, int screenWidth, int screenHeight)
		{
			ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_None;

			float yPos = ImGui::GetFrameHeight() * 2 + 8;
			ImGui::DockBuilderSetNodePos(dockspaceID, ImVec2(0.0f, yPos));
			ImGui::DockBuilderSetNodeSize(dockspaceID, ImVec2(screenWidth, screenHeight - yPos));
		}

		void UI::initParticleLayout(ImGuiID dockspaceID, int width, int height)
		{
			ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_None;

			if (!ImGui::DockBuilderGetNode(dockspaceID))
			{
				ImGui::DockBuilderRemoveNode(dockspaceID);
				ImGui::DockBuilderAddNode(dockspaceID, dockNodeFlags);
				ImGui::DockBuilderSetNodeSize(dockspaceID, ImVec2(width, height - ImGui::GetFrameHeight()));

				ImGuiID mainBtmID, midID, midBtmID, rightID, rightBtmID, extraID;
				ImGuiID mainID = dockspaceID;
				ImGui::DockBuilderSplitNode(mainID, ImGuiDir_Right, 0.8f, &midID, &mainID);
				ImGui::DockBuilderSplitNode(mainID, ImGuiDir_Down, 0.5f, &mainBtmID, &mainID);
				ImGui::DockBuilderSplitNode(midID, ImGuiDir_Down, 0.3f, &midBtmID, &midID);
				ImGui::DockBuilderSplitNode(midID, ImGuiDir_Right, 0.2f, &rightID, &midID);

				ImGui::DockBuilderDockWindow(effWindow, mainID);
				ImGui::DockBuilderDockWindow(matWindow, mainBtmID);
				ImGui::DockBuilderDockWindow(gPlayerWindow, midID);
				ImGui::DockBuilderDockWindow(inspectorWindow, rightID);
				ImGui::DockBuilderDockWindow(pTimelineWindow, midBtmID);

				ImGui::DockBuilderFinish(mainID);
			}
		}

		void UI::initModelLayout(ImGuiID dockspaceID, int width, int height)
		{
			ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_None;

			if (!ImGui::DockBuilderGetNode(dockspaceID))
			{
				ImGui::DockBuilderRemoveNode(dockspaceID);
				ImGui::DockBuilderAddNode(dockspaceID, dockNodeFlags);
				ImGui::DockBuilderSetNodeSize(dockspaceID, ImVec2(width, height - ImGui::GetFrameHeight()));

				ImGuiID mainBtmID, midID, midBtmID, rightID, rightBtmID, extraID;
				ImGuiID mainID = dockspaceID;
				ImGui::DockBuilderSplitNode(mainID, ImGuiDir_Right, 0.8f, &midID, &mainID);
				ImGui::DockBuilderSplitNode(mainID, ImGuiDir_Down, 0.5f, &mainBtmID, &mainID);
				ImGui::DockBuilderSplitNode(midID, ImGuiDir_Down, 0.3f, &midBtmID, &midID);
				ImGui::DockBuilderSplitNode(midID, ImGuiDir_Right, 0.2f, &rightID, &midID);

				ImGui::DockBuilderDockWindow(modelWindow, mainID);
				ImGui::DockBuilderDockWindow(texWindow, mainBtmID);
				ImGui::DockBuilderDockWindow(mViewerWindow, midID);
				ImGui::DockBuilderDockWindow(mPropertiesWindow, rightID);
				ImGui::DockBuilderDockWindow(mTimelineWindow, midBtmID);

				ImGui::DockBuilderFinish(mainID);
			}
		}

		bool UI::transparentButton(const std::string& lbl, const ImVec2& size)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
			//ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
			//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
			bool clicked = ImGui::Button(lbl.c_str(), size);

			ImGui::PopStyleColor(1);
			return clicked;
		}

		bool UI::selectNode(int& selection1, int val1, int& selection2, int val2)
		{
			bool result = false;
			if (ImGui::IsItemActivated())
			{
				selection1 = val1;
				selection2 = val2;
				result = true;
			}

			return result;
		}

		bool UI::isNodeSelected(int selection1, int val1, int selection2, int val2)
		{
			return selection1 == val1 && selection2 == val2;
		}
	}
}

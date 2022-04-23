#include "UI.h"
#include "ImGui/imgui_internal.h"

namespace Glitter
{
	namespace Editor
	{
		const char* UI::effWindow		= ICON_FA_MAGIC " Effects";
		const char* UI::matWindow		= ICON_FA_LAYER_GROUP " Materials";
		const char* UI::pTimelineWindow = ICON_FA_CLOCK " Animation##particle";
		const char* UI::gPlayerWindow	= ICON_FA_IMAGE " Preview";
		const char* UI::inspectorWindow = ICON_FA_INFO_CIRCLE " Inspector";

		float UI::btnHeight = 23.0f;
		ImVec2 UI::btnSmall = ImVec2(btnHeight - 5, btnHeight - 5);
		ImVec2 UI::btnNormal = ImVec2(btnHeight, btnHeight);
		ImVec2 UI::btnWide = ImVec2(50.0f, btnHeight);

		void UI::itemRowsBackground(float lineHeight, const ImColor& color)
		{
			auto* drawList = ImGui::GetWindowDrawList();
			const auto& style = ImGui::GetStyle();

			if (lineHeight < 0)
				lineHeight = ImGui::GetFrameHeightWithSpacing();

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

		void UI::initLayout(ImGuiID dockspaceID)
		{
			dockspaceID = ImGui::DockSpaceOverViewport();
			if (ImGui::DockBuilderGetNode(dockspaceID))
				return;

			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID);
			ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetMainViewport()->WorkSize);
			ImGui::DockBuilderFinish(dockspaceID);
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
			if (ImGui::IsItemActivated() && !ImGui::IsItemToggledOpen())
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

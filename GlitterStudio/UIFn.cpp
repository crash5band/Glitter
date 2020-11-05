#include "UIFn.h"
#include "ImGui/imgui_internal.h"

void itemRowsBackground(float lineHeight, const ImColor& color)
{
	auto* drawList = ImGui::GetWindowDrawList();
	const auto& style = ImGui::GetStyle();

	if (lineHeight < 0)
	{
		lineHeight = ImGui::GetFrameHeight();
	}
	lineHeight += style.ItemSpacing.y;

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

void resizeLayout(ImGuiID dockspaceID, int screenWidth, int screenHeight)
{
	ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_CentralNode;
	dockspaceID = ImGui::GetID("Dummy");

	ImGui::DockBuilderSetNodePos(dockspaceID, ImVec2(0.0f, ImGui::GetFrameHeight()));
	ImGui::DockBuilderSetNodeSize(dockspaceID, ImVec2(screenWidth, screenHeight - ImGui::GetFrameHeight()));
}

void initLayout(ImGuiID dockspaceID)
{
	ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_CentralNode;
	dockspaceID = ImGui::GetID("Dummy");

	if (!ImGui::DockBuilderGetNode(dockspaceID))
	{
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, dockNodeFlags);

		ImGuiID mainBtmID, midID, midBtmID, rightID, rightBtmID, extraID;
		ImGuiID mainID = dockspaceID;
		ImGui::DockBuilderSplitNode(mainID, ImGuiDir_Right, 0.77f, &midID, &mainID);
		ImGui::DockBuilderSplitNode(mainID, ImGuiDir_Down, 0.5f, &mainBtmID, &mainID);
		ImGui::DockBuilderSplitNode(midID, ImGuiDir_Right, 0.32f, &rightID, &midID);
		ImGui::DockBuilderSplitNode(midID, ImGuiDir_Down, 0.35f, &midBtmID, &midID);
		ImGui::DockBuilderSplitNode(rightID, ImGuiDir_Down, 0.35f, &rightBtmID, &rightID);
		ImGui::DockBuilderSplitNode(mainBtmID, ImGuiDir_Down, 0.35f, &extraID, &mainBtmID);

		ImGui::DockBuilderDockWindow(effWindow, mainID);
		ImGui::DockBuilderDockWindow(matWindow, mainBtmID);
		ImGui::DockBuilderDockWindow(logWindow, extraID);
		ImGui::DockBuilderDockWindow(debugWindow, extraID);
		ImGui::DockBuilderDockWindow(historyWindow, extraID);
		ImGui::DockBuilderDockWindow(prevWindow, midID);
		ImGui::DockBuilderDockWindow(propertiesWindow, rightID);
		ImGui::DockBuilderDockWindow(matProperties, rightID);
		ImGui::DockBuilderDockWindow(animProperties, rightBtmID);
		ImGui::DockBuilderDockWindow(curveEditor, midBtmID);

		ImGui::DockBuilderFinish(mainID);
	}
}
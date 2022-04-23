#pragma once
#include "IconsFontAwesome5.h"
#include "ImGui/imgui.h"
#include <vector>
#include <string>

namespace Glitter
{
	namespace Editor
	{
		class UI
		{
		public:
			static const char* effWindow;
			static const char* matWindow;
			static const char* inspectorWindow;
			static const char* pTimelineWindow;
			static const char* gPlayerWindow;

			static float btnHeight;
			static ImVec2 btnSmall;
			static ImVec2 btnNormal;
			static ImVec2 btnWide;

			static void initLayout(ImGuiID dockspaceID);
			static void itemRowsBackground(float lineHeight = -1.0f, const ImColor& color = ImColor(55, 55, 55, 255));
			static bool transparentButton(const std::string& lbl, const ImVec2& size);
			static bool selectNode(int& selection1, int val1, int& selection2, int val2);
			static bool isNodeSelected(int selection1, int val1, int selection2, int val2);
		};
	}
}


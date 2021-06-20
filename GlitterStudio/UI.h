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
			static const char* modelWindow;
			static const char* texWindow;
			static const char* mViewerWindow;
			static const char* mPropertiesWindow;
			static const char* mTimelineWindow;

			static float btnHeight;
			static ImVec2 btnSmall;
			static ImVec2 btnNormal;
			static ImVec2 btnWide;

			static void initParticleLayout(ImGuiID dockspaceID, int width, int height);
			static void initModelLayout(ImGuiID dockspaceID, int width, int height);
			static void itemRowsBackground(float lineHeight = -1.0f, const ImColor& color = ImColor(55, 55, 55, 255));
			static void resizeLayout(ImGuiID dockspaceID, int screenWidth, int screenHeight);
			static bool transparentButton(const std::string& lbl, const ImVec2& size);
			static bool selectNode(int& selection1, int val1, int& selection2, int val2);
			static bool isNodeSelected(int selection1, int val1, int selection2, int val2);
		};
	}
}


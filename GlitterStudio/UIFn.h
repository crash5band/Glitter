#pragma once
#include "IconsFontAwesome5.h"
#include "ImGui/imgui.h"

constexpr const char* effWindow = ICON_FA_MAGIC " Effects";
constexpr const char* matWindow = ICON_FA_CHESS_BOARD " Materials";
constexpr const char* prevWindow = ICON_FA_IMAGE " Preview";
constexpr const char* propertiesWindow = ICON_FA_INFO_CIRCLE " Properties";
constexpr const char* matProperties = ICON_FA_LAYER_GROUP " Material Properties";
constexpr const char* animProperties = ICON_FA_INFO_CIRCLE " Animation Properties";
constexpr const char* curveEditor = ICON_FA_CHART_BAR " Curve Editor";
constexpr const char* debugWindow = ICON_FA_BUG " Debug Info";
constexpr const char* historyWindow = ICON_FA_HISTORY " Command History";
constexpr const char* logWindow = ICON_FA_CLIPBOARD " Log";

void itemRowsBackground(float lineHeight = -1.0f, const ImColor& color = ImColor(55, 55, 55, 255));
void resizeLayout(ImGuiID dockspaceID, int screenWidth, int screenHeight);
void initLayout(ImGuiID dockspaceID);
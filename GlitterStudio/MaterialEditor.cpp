#include "MaterialEditor.h"
#include "IconsFontAwesome5.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "UiHelper.h"
#include "UIFn.h"

std::vector<std::shared_ptr<MaterialNode>> MaterialEditor::materialNodes;
int MaterialEditor::index = -1;
float MaterialEditor::texSize = 128.0f;
bool MaterialEditor::showSplits = false;

void MaterialEditor::add(std::shared_ptr<MaterialNode>& node)
{
	materialNodes.emplace_back(node);
}

void MaterialEditor::remove(size_t pos)
{
	materialNodes.erase(materialNodes.begin() + pos);
}

void MaterialEditor::clear()
{
	materialNodes.clear();
}

std::vector<std::shared_ptr<MaterialNode>> MaterialEditor::getNodes()
{
	return materialNodes;
}

int MaterialEditor::getSelection()
{
	return index;
}

void MaterialEditor::update()
{
	ImGuiTreeNodeFlags basicFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_FramePadding;
	
	if (ImGui::Begin(matWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 2));
		itemRowsBackground();

		for (size_t m = 0; m < materialNodes.size(); ++m)
		{
			ImGuiTreeNodeFlags nodeFlags = basicFlags;
			if (m == index)
				nodeFlags |= ImGuiTreeNodeFlags_Selected;

			ImGui::TreeNodeEx((void*)(intptr_t)m, nodeFlags, "%s %s", ICON_FA_CUBE, materialNodes[m]->getMaterial()->getName().c_str());
			if (ImGui::IsItemClicked())
				index = m;
		}

		ImGui::PopStyleVar(2);
	}
	ImGui::End();

	if (ImGui::Begin(matProperties, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		if (index > -1 && index < materialNodes.size() && materialNodes.size())
		{
			materialNodes[index]->populateInspector();

			ImGui::Separator();
			if (ImGui::TreeNodeEx("Preview", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Show Splits", &showSplits);

				auto tex = materialNodes[index]->getTexture();
				if (tex)
				{
					ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - 64);
					ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 10);

					ImVec2 min = ImGui::GetCursorScreenPos();
					ImDrawList* drawList = ImGui::GetWindowDrawList();

					drawList->AddRectFilled(ImVec2(min.x - 5, min.y - 5), ImVec2(min.x + texSize + 5, min.y + texSize + 5), ImGui::GetColorU32(ImVec4(0.15f, 0.15f, 0.15f, 0.85f)), 0.25f);
					ImGui::Image((void*)tex->getID(), ImVec2(texSize, texSize), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1));

					if (showSplits)
					{
						Glitter::Vector2 splits = materialNodes[index]->getMaterial()->getSplit();
						const int x = splits.x;
						const int y = splits.y;

						ImU32 color = ImGui::GetColorU32(ImVec4(0.95f, 0.0f, 0.0f, 0.85f));

						if (x > 1)
						{
							float factor = texSize / (float)x;
							for (int w = 1; w < x; ++w)
							{
								float pX = min.x + (factor * w);
								drawList->AddLine(ImVec2(pX, min.y), ImVec2(pX, min.y + texSize), color);
							}
						}

						if (y > 1)
						{
							float factor = texSize / (float)y;
							for (int h = 1; h < y; ++h)
							{
								float pY = min.y + (factor * h);
								drawList->AddLine(ImVec2(min.x, pY), ImVec2(min.x + texSize, pY), color);
							}
						}
					}
				}

				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::Text("Select a material to view its properties.");
		}
	}
	ImGui::End();
}
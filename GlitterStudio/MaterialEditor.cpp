#include "MaterialEditor.h"
#include "IconsFontAwesome5.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "UiHelper.h"
#include "UIFn.h"
#include "Logger.h"
#include "ResourceManager.h"

std::vector<std::shared_ptr<MaterialNode>> MaterialEditor::materialNodes;
int MaterialEditor::selection = -1;
float MaterialEditor::texSize = 128.0f;
bool MaterialEditor::showSplits = false;

void MaterialEditor::add(std::shared_ptr<MaterialNode>& node)
{
	materialNodes.emplace_back(node);
}

void MaterialEditor::remove(size_t pos)
{
	std::string matName = materialNodes.at(pos)->getMaterial()->getName();
	if (materialNodes.at(pos).use_count() < 2)
	{
		materialNodes.erase(materialNodes.begin() + pos);
		ResourceManager::cleanTextures();
		Logger::log(Message(MessageType::Normal, "Closed material" + matName + "."));

		selection = -1;
	}
	else
	{
		std::string warning = "Cannot close " + matName + ". The material is used by on open effect.";
		Logger::log(Message(MessageType::Warning, warning));
	}
}

void MaterialEditor::clean()
{
	// Dispose of unused materials
	for (std::vector<std::shared_ptr<MaterialNode>>::iterator it = materialNodes.begin(); it != materialNodes.end();)
	{
		if ((*it).use_count() < 2)
		{
			std::string matName = (*it)->getMaterial()->getName();
			it = materialNodes.erase(it);
			selection = -1;

			Logger::log(Message(MessageType::Normal, "Closed material " + matName + "."));
		}
		else
			++it;
	}
}

void MaterialEditor::clear()
{
	materialNodes.clear();
	ResourceManager::cleanTextures();
	selection = -1;
}

std::vector<std::shared_ptr<MaterialNode>> MaterialEditor::getNodes()
{
	return materialNodes;
}

int MaterialEditor::getSelection()
{
	return selection;
}

int MaterialEditor::count()
{
	return materialNodes.size();
}

void MaterialEditor::select(int index)
{
	if (index >= 0 && index < MaterialEditor::count())
		selection = index;
	else
		selection = MaterialEditor::count() - 1;
}

void MaterialEditor::update()
{
	ImGuiTreeNodeFlags basicFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_FramePadding;
	
	if (ImGui::Begin(matWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		itemRowsBackground();
		for (size_t m = 0; m < materialNodes.size(); ++m)
		{
			ImGuiTreeNodeFlags nodeFlags = basicFlags;
			if (m == selection)
				nodeFlags |= ImGuiTreeNodeFlags_Selected;

			ImGui::TreeNodeEx((void*)(intptr_t)m, nodeFlags, "%s %s", ICON_FA_SQUARE, materialNodes[m]->getMaterial()->getName().c_str());
			if (ImGui::IsItemActivated())
				selection = m;

			if (ImGui::IsItemHovered())
			{
				auto tex = materialNodes[m]->getTexture();
				if (tex)
				{
					ImGui::BeginTooltip();
					ImGui::Image((void*)tex->getID(), ImVec2(texSize, texSize), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1));
					ImGui::EndTooltip();
				}
			}

			if (materialMenu())
			{
				--m;
				continue;
			}
		}
	}

	ImGui::End();

	if (ImGui::Begin(matProperties, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		if (selection > -1 && selection < materialNodes.size() && materialNodes.size())
		{
			materialNodes[selection]->populateInspector();

			ImGui::Separator();
			if (ImGui::TreeNodeEx("Preview", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Show Splits", &showSplits);
				preview();

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

void MaterialEditor::preview()
{
	auto tex = materialNodes[selection]->getTexture();
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
			Glitter::Vector2 splits = materialNodes[selection]->getMaterial()->getSplit();
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
}

bool MaterialEditor::materialMenu()
{
	bool closed = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Selectable(ICON_FA_TIMES " Close"))
		{
			remove(selection);
			closed = true;
		}

		ImGui::EndPopup();
	}

	return closed;
}

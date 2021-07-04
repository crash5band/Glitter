#include "Inspector.h"
#include "TextureData.h"
#include "MaterialNode.h"
#include "UI.h"

namespace Glitter
{
	namespace Editor
	{
		Inspector::Inspector() : texSize{ 128 }, showTextureSplits{ true }
		{
			node.lock() = nullptr;
		}

		void Inspector::setNode(std::shared_ptr<INode> n)
		{
			node = n;
		}

		void Inspector::update()
		{
			if (ImGui::Begin(ICON_FA_INFO_CIRCLE " Inspector"))
			{
				UI::transparentButton(ICON_FA_WRENCH, UI::btnNormal);
				if (ImGui::BeginPopupContextItem("inspector_settings", ImGuiMouseButton_Left))
				{
					ImGui::Checkbox("Show Texture Splits", &showTextureSplits);

					ImGui::EndPopup();
				}

				std::shared_ptr<INode> n = node.lock();
				if (n)
				{
					n->populateInspector();
					if (n->getNodeType() == NodeType::GTMaterial)
					{
						if (ImGui::TreeNodeEx("Preview", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
						{
							std::shared_ptr<MaterialNode> material = std::static_pointer_cast<MaterialNode>(n);
							previewTexture(material->getTexture(), material->getMaterial()->getSplit(), showTextureSplits, true);

							ImGui::TreePop();
						}
					}
				}
				else
				{
					ImGui::Text("Select a node to view its properties.");
				}
			}

			ImGui::End();
		}

		void Inspector::previewTexture(std::shared_ptr<TextureData> texture, Glitter::Vector2 split, bool showSplits, bool limitSizeByWindow)
		{
			if (!texture)
				return;

			float width = texture->getWidth();
			float height = texture->getHeight();
			float ratio = width / height;
			float maxSize = width;
			if (height > width)
				maxSize = height;

			if (limitSizeByWindow)
			{
				if (maxSize > ImGui::GetWindowSize().x - 50.0f)
					maxSize = ImGui::GetWindowSize().x - 50.0f;
			}
			else
			{
				maxSize = texSize;
			}

			if (ratio < 1)
			{
				height = maxSize;
				width = ratio * maxSize;
			}
			else if (ratio > 1)
			{
				ratio = height / width;
				width = maxSize;
				height = ratio * maxSize;
			}
			else
			{
				width = height = maxSize;
			}

			ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - (width / 2.0f));
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 10);

			ImVec2 min = ImGui::GetCursorScreenPos();
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			drawList->AddRectFilled(ImVec2(min.x - 5, min.y - 5), ImVec2(min.x + width + 5, min.y + height + 5), ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)), 0.25f);
			ImGui::Image((void*)texture->getID(), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1));

			if (showSplits)
			{
				const int x = split.x;
				const int y = split.y;

				ImU32 color = ImGui::GetColorU32(ImVec4(0.8f, 0.1f, 0.1f, 0.85f));

				if (x > 1)
				{
					float factor = width / (float)x;
					for (int w = 1; w < x; ++w)
					{
						float pX = min.x + (factor * w);
						drawList->AddLine(ImVec2(pX, min.y), ImVec2(pX, min.y + height), color);
					}
				}

				if (y > 1)
				{
					float factor = height / (float)y;
					for (int h = 1; h < y; ++h)
					{
						float pY = min.y + (factor * h);
						drawList->AddLine(ImVec2(min.x, pY), ImVec2(min.x + width, pY), color);
					}
				}
			}
		}
	}
}
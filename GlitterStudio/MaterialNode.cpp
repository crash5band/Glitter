#include "MaterialNode.h"
#include "UiHelper.h"
#include "File.h"
#include "FileGUI.h"
#include "ResourceManager.h"

namespace Glitter
{
	namespace Editor
	{
		MaterialNode::MaterialNode(std::shared_ptr<GlitterMaterial>& mat) :
			material{ mat }
		{
			const std::string path = File::getFilePath(mat->getFilename());
			const std::string filename = std::string(path) + mat->getTexture() + ".dds";
			texture = nullptr;

			if (mat->getTexture().size())
			{
				ResourceManager::loadTexture(filename, TextureSlot::Diffuse);
				changeTexture(ResourceManager::getTexture(File::getFileName(filename)));
			}
		}

		MaterialNode::MaterialNode(std::shared_ptr<MaterialNode>& rhs)
		{
			material = std::make_shared<GlitterMaterial>(*rhs->material);
			material->setFilename("");
			texture = rhs->texture;
		}

		std::shared_ptr<GlitterMaterial> MaterialNode::getMaterial()
		{
			return material;
		}

		std::shared_ptr<TextureData> MaterialNode::getTexture()
		{
			return texture;
		}

		NodeType MaterialNode::getNodeType()
		{
			return NodeType::GTMaterial;
		}

		void MaterialNode::changeTexture(std::shared_ptr<TextureData> tex)
		{
			texture = tex;
			if (tex) material->setTexture(tex->getName());
		}

		void MaterialNode::populateInspector()
		{
			using Material = GlitterMaterial;
			ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

			if (ImGui::TreeNodeEx("Material", treeFlags))
			{
				beginPropertyColumn();

				addTextProperty("Name", material->getName(), material, std::mem_fn(&Material::setName));

				std::string textureLbl = material->getTexture();
				if (!texture.get())
					textureLbl.append(" (Not Loaded)");

				ImGui::Text("Texture");
				ImGui::NextColumn();
				if (ImGui::Button(textureLbl.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, UI::btnHeight)))
				{
					std::string name;
					if (FileGUI::openFileGUI(FileType::Texture, name))
					{
						ResourceManager::loadTexture(name, TextureSlot::Diffuse);
						changeTexture(ResourceManager::getTexture(File::getFileName(name)));
					}
				}
				ImGui::NextColumn();

				addComboBoxProperty("Blend Mode", blendModeTable, blendModeTableSize, material->getBlendMode(),
					material, std::mem_fn(&Material::setBlendMode));

				addComboBoxProperty("Address Mode", addressModeTable, addressModeTableSize, material->getAddressMode(),
					material, std::mem_fn(&Material::setAddressMode));

				addVector2Property("Split", material->getSplit(), material, std::mem_fn(&Material::setSplit));

				ImGui::TreePop();
			}
			endPropertyColumn();

			if (ImGui::TreeNodeEx("Shader", treeFlags))
			{
				Shader& shader = material->getShader();

				beginPropertyColumn();
				ImGui::Text("Name");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				ImGui::InputText("##shader_name", &shader.name);
				endPropertyColumn();

				/*for (int i = 0; i < 4; ++i)
				{
					std::string paramIDName = "ID##" + std::to_string(i);
					std::string paramValueName = "Value##" + std::to_string(i);
					int id = shader.parameters[i].id;

					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 3);
					if (ImGui::InputInt(paramIDName.c_str(), &id, 1, 2))
						shader.parameters[i].id = id;

					ImGui::SameLine();

					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 2.11);
					ImGui::SliderFloat(paramValueName.c_str(), &shader.parameters[i].value, 0.0f, 1.0f, "%.2f");
					shader.parameters[i].value = std::clamp(shader.parameters[i].value, 0.0f, 1.0f);
				}*/

				ImGui::TreePop();
			}
		}

		std::shared_ptr<EditorAnimationSet> MaterialNode::getAnimationSet()
		{
			return nullptr;
		}
	}
}

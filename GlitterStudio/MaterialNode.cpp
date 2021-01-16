#include "MaterialNode.h"
#include "UiHelper.h"
#include "File.h"
#include "FileGUI.h"
#include "ResourceManager.h"

MaterialNode::MaterialNode(std::shared_ptr<Glitter::GlitterMaterial>& mat) :
	material{ mat }
{
	const std::string path = Glitter::File::getFilePath(mat->getFilename());
	const std::string texturePath = std::string(path) + mat->getTexture() + ".dds";
	texture = nullptr;
	if (mat->getTexture().size())
		changeTexture(texturePath);
}

std::shared_ptr<Glitter::GlitterMaterial> MaterialNode::getMaterial()
{
	return material;
}

std::shared_ptr<TextureData> MaterialNode::getTexture()
{
	return texture;
}

NodeType MaterialNode::getType()
{
	return NodeType::Material;
}

float MaterialNode::getLife()
{
	return 0.0f;
}

void MaterialNode::changeTexture(const std::string& filepath)
{
	ResourceManager::loadTexture(filepath, TextureSlot::Diffuse);
	texture = ResourceManager::getTexture(Glitter::File::getFileName(filepath));
	material->setTexture(Glitter::File::getFileNameWithoutExtension(filepath));
}

void MaterialNode::populateInspector()
{
	using Material = Glitter::GlitterMaterial;
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

	if (ImGui::TreeNodeEx("Material", treeFlags))
	{
		addTextProperty("Name", material->getName(), material, std::mem_fn(&Material::setName));

		beginPropertyColumn("Texture");
		if (ImGui::Button(texture.get() ? material->getTexture().c_str() : "None", ImVec2(ImGui::GetContentRegionAvail().x, btnHeight)))
		{
			std::string name;
			if (FileGUI::openFileGUI(FileType::Texture, name))
				changeTexture(name);
		}
		ImGui::NextColumn();

		addComboBoxProperty("Blend Mode", Glitter::blendModeTable, Glitter::blendModeTableSize - 1, material->getBlendMode(),
			material, std::mem_fn(&Material::setBlendMode));

		addComboBoxProperty("Address Mode", Glitter::addressModeTable, Glitter::addressModeTableSize, material->getAddressMode(),
			material, std::mem_fn(&Material::setAddressMode));

		addVector2Property("Split", material->getSplit(), material, std::mem_fn(&Material::setSplit));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Shader", treeFlags))
	{
		Glitter::Shader &shader = material->getShader();

		beginPropertyColumn("Name");
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

std::shared_ptr<AnimationNode> MaterialNode::getAnimationNode()
{
	return nullptr;
}

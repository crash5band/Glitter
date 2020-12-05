#include "ParticleNode.h"
#include "UiHelper.h"
#include "File.h"
#include "FileGUI.h"
#include "ResourceManager.h"

ParticleNode::ParticleNode(std::shared_ptr<Glitter::Particle>& p) :
	particle{ p }
{
	animationNode = std::make_shared<AnimationNode>(&p->getAnimations());
	mesh = nullptr;
}

std::shared_ptr<Glitter::Particle> &ParticleNode::getParticle()
{
	return particle;
}

std::shared_ptr<MaterialNode> &ParticleNode::getMaterialNode()
{
	return materialNode;
}

std::shared_ptr<ModelData> ParticleNode::getMesh()
{
	return mesh;
}

NodeType ParticleNode::getType()
{
	return NodeType::Particle;
}

std::shared_ptr<AnimationNode> ParticleNode::getAnimationNode()
{
	return animationNode;
}

void ParticleNode::setMaterial(std::shared_ptr<MaterialNode>& matNode)
{
	materialNode = matNode;
	initMesh();
}

void ParticleNode::changeMaterial(std::shared_ptr<MaterialNode>& matNode)
{
	materialNode = matNode;
	particle->setMaterial(materialNode->getMaterial()->getName());
}

void ParticleNode::changeMesh(const std::string& filepath)
{
	ResourceManager::loadModel(filepath);
	mesh = ResourceManager::getModel(Glitter::File::getFileName(filepath));
	particle->setMeshName(Glitter::File::getFileNameWithoutExtension(filepath));
}

void ParticleNode::initMesh()
{
	if (!materialNode)
		return;

	std::string materialPath = Glitter::File::getFilePath(materialNode->getMaterial()->getFilename());
	if (particle->getType() == Glitter::ParticleType::Mesh)
	{
		ResourceManager::loadModel(materialPath + "\\" + particle->getMeshName() + ".model");
		mesh = ResourceManager::getModel(particle->getMeshName() + ".model");
	}
}

Glitter::Color ParticleNode::getBaseColor() const
{
	return baseColor;
}

void ParticleNode::setBaseColor(Glitter::Color color)
{
	baseColor = color;
}

void ParticleNode::populateInspector()
{
	using Particle = Glitter::Particle;
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_FramePadding;
	ImGuiTreeNodeFlags defaultOpen = ImGuiTreeNodeFlags_DefaultOpen | treeFlags;

	if (ImGui::TreeNodeEx("Particle", defaultOpen))
	{
		addTextProperty("Name", particle->getName(), particle, std::mem_fn(&Particle::setName));
		addComboBoxProperty("Type", Glitter::particleTypeTable, Glitter::particleTypeTableSize, particle->getType(),
			particle, std::mem_fn(&Particle::setType));

		addFloatProperty("Life", particle->getLifeTime(), particle, std::mem_fn(&Particle::setLifeTime));
		addColorProperty("Color", particle->getColor(), particle, std::mem_fn(&Particle::setColor));
		addIntProperty("Max Count", particle->getMaxCount(), particle, std::mem_fn(&Particle::setMaxCount));

		addComboBoxProperty("Pivot", Glitter::pivotPositionTable, Glitter::pivotPositionTableSize, particle->getPivotPosition(),
			particle, std::mem_fn(&Particle::setPivotPosition));

		addComboBoxProperty("Direction Type", Glitter::PdirectionTypeTable, Glitter::PdirectionTypeTableSize,
			particle->getDirectionType(), particle, std::mem_fn(&Particle::setDirectionType));

		addFloatProperty("Z Offset", particle->getZOffset(), particle, std::mem_fn(&Particle::setZOffset));

		addVector3Property("Size", particle->getSize(), particle, std::mem_fn(&Particle::setSize));
		addVector3Property("Size Random", particle->getSizeRandom(), particle, std::mem_fn(&Particle::setSizeRandom));
		addVector3Property("Rotation", particle->getRotation(), particle, std::mem_fn(&Particle::setRotation));
		addVector3Property("Rotation Random", particle->getRotationRandom(), particle, std::mem_fn(&Particle::setRotationRandom));
		addVector3Property("Rotation Add", particle->getRotationAdd(), particle, std::mem_fn(&Particle::setRotationAdd));
		addVector3Property("Rotation Add Random", particle->getRotationAddRandom(), particle, std::mem_fn(&Particle::setRotationAddRandom));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Force", treeFlags))
	{
		addVector3Property("Direction", particle->getDirection(), particle, std::mem_fn(&Particle::setDirection));
		addVector3Property("Direction Random", particle->getDirectionRandom(), particle, std::mem_fn(&Particle::setDirectionRandom));
		addFloatProperty("Speed", particle->getSpeed(), particle, std::mem_fn(&Particle::setSpeed));
		addFloatProperty("Speed Random", particle->getSpeedRandom(), particle, std::mem_fn(&Particle::setSpeedRandom));
		
		ImGui::Separator();

		addVector3Property("Gravity", particle->getGravitationalAccel(), particle, std::mem_fn(&Particle::setGravitationalAccel));
		addVector3Property("Acceleration", particle->getExternalAccel(), particle, std::mem_fn(&Particle::setExternalAccel));
		addVector3Property("Acceleration Random", particle->getExternalAccelRandom(), particle, std::mem_fn(&Particle::setExternalAccelRandom));
		addFloatProperty("Deceleration", particle->getDeceleration(), particle, std::mem_fn(&Particle::setDeceleration));
		addFloatProperty("Deceleration Random", particle->getDecelerationRandom(), particle, std::mem_fn(&Particle::setDecelerationRandom));
		
		ImGui::Separator();

		addFloatProperty("Reflection Coeff", particle->getReflectionCoeff(), particle, std::mem_fn(&Particle::setReflectionCoeff));
		addFloatProperty("Reflection Coeff Random", particle->getReflectionCoeffRandom(), particle, std::mem_fn(&Particle::setReflectionCoeffRandom));
		addFloatProperty("Rebound Plane Y", particle->getReboundPlaneY(), particle, std::mem_fn(&Particle::setReboundPlaneY));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (particle->getType() == Glitter::ParticleType::Mesh)
	{
		if (ImGui::TreeNodeEx("Mesh", defaultOpen))
		{
			beginPropertyColumn("Mesh");
			if (ImGui::Button(mesh.get() ? particle->getMeshName().c_str() : "None", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight())))
			{
				std::string name;
				if (FileGUI::openFileGUI(FileType::Model, name))
					changeMesh(name);
			}
			ImGui::NextColumn();
			ImGui::TreePop();
		}
		endPropertyColumn();
	}
	else if (particle->getType() == Glitter::ParticleType::Locus)
	{
		if (ImGui::TreeNodeEx("Locus Properties", defaultOpen))
		{
			addUIntProperty("Locus History Size", particle->getLocusHistorySize(), particle, std::mem_fn(&Particle::setLocusHistorySize));
			addUIntProperty("Locus History Size Random", particle->getLocusHistorySize(), particle, std::mem_fn(&Particle::setLocusHistorySizeRandom));
			ImGui::TreePop();
		}
		endPropertyColumn();
	}

	if (ImGui::TreeNodeEx("Texture", defaultOpen))
	{
		std::string comboLbl = materialNode ? materialNode->getMaterial()->getName() : std::string(particle->getMaterial() + "(Not Loaded)");
		std::vector<std::shared_ptr<MaterialNode>> materials = MaterialEditor::getNodes();
		static int selectedIndex = -1;

		beginPropertyColumn("Material");
		if (ImGui::BeginCombo("##material_list", comboLbl.c_str()))
		{
			for (int n = 0; n < materials.size(); ++n)
			{
				const bool selected = selectedIndex == n;
				if (ImGui::Selectable(materials[n]->getMaterial()->getName().c_str(), selected))
				{
					selectedIndex = n;
					changeMaterial(materials[n]);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		ImGui::NextColumn();

		addUIntProperty("Texture Index", particle->getTextureIndex(), particle, std::mem_fn(&Particle::setTextureIndex));
		addComboBoxProperty("UV Index Type", Glitter::uvIndexTypeTable, Glitter::uvIndexTypeTableSize, particle->getUVIndexType(),
			particle, std::mem_fn(&Particle::setUVIndexType));

		addUIntProperty("UV Index", particle->getUVIndex(), particle, std::mem_fn(&Particle::setUVIndex));
		addFloatProperty("UV Change Interval", particle->getUVChangeInterval(), particle, std::mem_fn(&Particle::setUVChangeInterval));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Color", treeFlags))
	{
		addVector2Property("Color Scroll", particle->getColorScroll(), particle, std::mem_fn(&Particle::setColorScroll));
		addVector2Property("Color Scroll Random", particle->getColorScrollRandom(), particle, std::mem_fn(&Particle::setColorScrollRandom));
		addFloatProperty("Color Scroll Speed", particle->getColorScrollSpeed(), particle, std::mem_fn(&Particle::setColorScrollSpeed));

		ImGui::Separator();

		addVector2Property("2nd Color Scroll", particle->getSecondaryColorScroll(), particle, std::mem_fn(&Particle::setSecondaryColorScroll));
		addVector2Property("2nd Color Scroll Random", particle->getSecondaryColorScrollRandom(), particle, std::mem_fn(&Particle::setSecondaryColorScrollRandom));
		addFloatProperty("2nd Color Scroll Speed", particle->getSecondaryColorScrollSpeed(), particle, std::mem_fn(&Particle::setSecondaryColorScrollSpeed));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Alpha", treeFlags))
	{
		addVector2Property("Alpha Scroll", particle->getAlphaScroll(), particle, std::mem_fn(&Particle::setAlphaScroll));
		addVector2Property("Alpha Scroll Random", particle->getAlphaScrollRandom(), particle, std::mem_fn(&Particle::setAlphaScrollRandom));
		addFloatProperty("Alpha Scroll Speed", particle->getAlphaScrollSpeed(), particle, std::mem_fn(&Particle::setAlphaScrollSpeed));

		ImGui::Separator();

		addVector2Property("2nd Alpha Scroll", particle->getSecondaryAlphaScroll(), particle, std::mem_fn(&Particle::setSecondaryAlphaScroll));
		addVector2Property("2nd Alpha Scroll Random", particle->getSecondaryAlphaScrollRandom(), particle, std::mem_fn(&Particle::setSecondaryAlphaScrollRandom));
		addFloatProperty("2nd Alpha Scroll Speed", particle->getSecondaryAlphaScrollSpeed(), particle, std::mem_fn(&Particle::setSecondaryAlphaScrollSpeed));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Emitter Translation", treeFlags))
	{
		addFloatProperty("Emitter-Effect Ratio", particle->getEmitterTranslationEffectRatio(), particle, std::mem_fn(&Particle::setEmitterTranslationEffectRatio));
		addFloatProperty("Follow Ratio", particle->getFollowEmitterTranslationRatio(), particle, std::mem_fn(&Particle::setFollowEmitterTranslationRatio));
		addFloatProperty("Follow Y Ratio", particle->getFollowEmitterTranslationYRatio(), particle, std::mem_fn(&Particle::setFollowEmitterTranslationYRatio));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Blending", treeFlags))
	{
		addComboBoxProperty("Blending Mode", Glitter::blendModeTable, Glitter::blendModeTableSize, particle->getBlendMode(),
			particle, std::mem_fn(&Particle::setBlendMode));

		addComboBoxProperty("2nd Blending Mode", Glitter::blendModeTable, Glitter::blendModeTableSize, particle->getSecondaryBlendMode(),
			particle, std::mem_fn(&Particle::setSecondaryBlendMode));

		addFloatProperty("2nd Blend", particle->getSecondaryBlend(), particle, std::mem_fn(&Particle::setSecondaryBlend));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Flags", defaultOpen))
	{
		addFlagsProperty("Emitter Local", particle->getFlags(), 4, particle, std::mem_fn(&Particle::setFlags));
		addFlagsProperty("Scale Using X Only", particle->getFlags(), 16, particle, std::mem_fn(&Particle::setFlags));
		addFlagsProperty("Z Test", particle->getFlags(), 64, particle, std::mem_fn(&Particle::setFlags));

		addUIntProperty("Flags", particle->getFlags(), particle, std::mem_fn(&Particle::setFlags));
		ImGui::TreePop();
	}
}

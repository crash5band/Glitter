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
	Glitter::Particle* p = particle.get();
	using Particle = Glitter::Particle;
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_FramePadding;
	ImGuiTreeNodeFlags defaultOpen = ImGuiTreeNodeFlags_DefaultOpen | treeFlags;

	if (ImGui::TreeNodeEx("Particle", defaultOpen))
	{
		addTextProperty("Name", p->getName(), p, std::mem_fn(&Particle::setName));
		addComboBoxProperty("Type", Glitter::particleTypeTable, Glitter::particleTypeTableSize, p->getType(),
			p, std::mem_fn(&Particle::setType));

		addFloatProperty("Life", p->getLifeTime(), p, std::mem_fn(&Particle::setLifeTime));
		addColorProperty("Color", p->getColor(), p, std::mem_fn(&Particle::setColor));
		addIntProperty("Max Count", p->getMaxCount(), p, std::mem_fn(&Particle::setMaxCount));

		addComboBoxProperty("Pivot", Glitter::pivotPositionTable, Glitter::pivotPositionTableSize, p->getPivotPosition(),
			p, std::mem_fn(&Particle::setPivotPosition));

		addComboBoxProperty("Direction Type", Glitter::PdirectionTypeTable, Glitter::PdirectionTypeTableSize,
			p->getDirectionType(), p, std::mem_fn(&Particle::setDirectionType));

		addFloatProperty("Z Offset", p->getZOffset(), p, std::mem_fn(&Particle::setZOffset));

		addVector3Property("Size", p->getSize(), p, std::mem_fn(&Particle::setSize));
		addVector3Property("Size Random", p->getSizeRandom(), p, std::mem_fn(&Particle::setSizeRandom));
		addVector3Property("Rotation", p->getRotation(), p, std::mem_fn(&Particle::setRotation));
		addVector3Property("Rotation Random", p->getRotationRandom(), p, std::mem_fn(&Particle::setRotationRandom));
		addVector3Property("Rotation Add", p->getRotationAdd(), p, std::mem_fn(&Particle::setRotationAdd));
		addVector3Property("Rotation Add Random", p->getRotationAddRandom(), p, std::mem_fn(&Particle::setRotationAddRandom));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Force", treeFlags))
	{
		addVector3Property("Direction", p->getDirection(), p, std::mem_fn(&Particle::setDirection));
		addVector3Property("Direction Random", p->getDirectionRandom(), p, std::mem_fn(&Particle::setDirectionRandom));
		addFloatProperty("Speed", p->getSpeed(), p, std::mem_fn(&Particle::setSpeed));
		addFloatProperty("Speed Random", p->getSpeedRandom(), p, std::mem_fn(&Particle::setSpeedRandom));
		
		ImGui::Separator();

		addVector3Property("Gravity", p->getGravitationalAccel(), p, std::mem_fn(&Particle::setGravitationalAccel));
		addVector3Property("Acceleration", p->getExternalAccel(), p, std::mem_fn(&Particle::setExternalAccel));
		addVector3Property("Acceleration Random", p->getExternalAccelRandom(), p, std::mem_fn(&Particle::setExternalAccelRandom));
		addFloatProperty("Deceleration", p->getDeceleration(), p, std::mem_fn(&Particle::setDeceleration));
		addFloatProperty("Deceleration Random", p->getDecelerationRandom(), p, std::mem_fn(&Particle::setDecelerationRandom));
		
		ImGui::Separator();

		addFloatProperty("Reflection Coeff", p->getReflectionCoeff(), p, std::mem_fn(&Particle::setReflectionCoeff));
		addFloatProperty("Reflection Coeff Random", p->getReflectionCoeffRandom(), p, std::mem_fn(&Particle::setReflectionCoeffRandom));
		addFloatProperty("Rebound Plane Y", p->getReboundPlaneY(), p, std::mem_fn(&Particle::setReboundPlaneY));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (p->getType() == Glitter::ParticleType::Mesh)
	{
		if (ImGui::TreeNodeEx("Mesh", defaultOpen))
		{
			beginPropertyColumn("Mesh");
			if (ImGui::Button(mesh.get() ? p->getMeshName().c_str() : "None", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight())))
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
	else if (p->getType() == Glitter::ParticleType::Locus)
	{
		if (ImGui::TreeNodeEx("Locus Properties", defaultOpen))
		{
			addUIntProperty("Locus History Size", p->getLocusHistorySize(), p, std::mem_fn(&Particle::setLocusHistorySize));
			addUIntProperty("Locus History Size Random", p->getLocusHistorySize(), p, std::mem_fn(&Particle::setLocusHistorySizeRandom));
			ImGui::TreePop();
		}
		endPropertyColumn();
	}

	if (ImGui::TreeNodeEx("Texture", defaultOpen))
	{
		std::string comboLbl = materialNode ? materialNode->getMaterial()->getName() : std::string(p->getMaterial() + "(Not Loaded)");
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

		addUIntProperty("Texture Index", p->getTextureIndex(), p, std::mem_fn(&Particle::setTextureIndex));
		addComboBoxProperty("UV Index Type", Glitter::uvIndexTypeTable, Glitter::uvIndexTypeTableSize, p->getUVIndexType(),
			p, std::mem_fn(&Particle::setUVIndexType));

		addUIntProperty("UV Index", p->getUVIndex(), p, std::mem_fn(&Particle::setUVIndex));
		addFloatProperty("UV Change Interval", p->getUVChangeInterval(), p, std::mem_fn(&Particle::setUVChangeInterval));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Color", treeFlags))
	{
		addVector2Property("Color Scroll", p->getColorScroll(), p, std::mem_fn(&Particle::setColorScroll));
		addVector2Property("Color Scroll Random", p->getColorScrollRandom(), p, std::mem_fn(&Particle::setColorScrollRandom));
		addFloatProperty("Color Scroll Speed", p->getColorScrollSpeed(), p, std::mem_fn(&Particle::setColorScrollSpeed));

		ImGui::Separator();

		addVector2Property("2nd Color Scroll", p->getSecondaryColorScroll(), p, std::mem_fn(&Particle::setSecondaryColorScroll));
		addVector2Property("2nd Color Scroll Random", p->getSecondaryColorScrollRandom(), p, std::mem_fn(&Particle::setSecondaryColorScrollRandom));
		addFloatProperty("2nd Color Scroll Speed", p->getSecondaryColorScrollSpeed(), p, std::mem_fn(&Particle::setSecondaryColorScrollSpeed));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Alpha", treeFlags))
	{
		addVector2Property("Alpha Scroll", p->getAlphaScroll(), p, std::mem_fn(&Particle::setAlphaScroll));
		addVector2Property("Alpha Scroll Random", p->getAlphaScrollRandom(), p, std::mem_fn(&Particle::setAlphaScrollRandom));
		addFloatProperty("Alpha Scroll Speed", p->getAlphaScrollSpeed(), p, std::mem_fn(&Particle::setAlphaScrollSpeed));

		ImGui::Separator();

		addVector2Property("2nd Alpha Scroll", p->getSecondaryAlphaScroll(), p, std::mem_fn(&Particle::setSecondaryAlphaScroll));
		addVector2Property("2nd Alpha Scroll Random", p->getSecondaryAlphaScrollRandom(), p, std::mem_fn(&Particle::setSecondaryAlphaScrollRandom));
		addFloatProperty("2nd Alpha Scroll Speed", p->getSecondaryAlphaScrollSpeed(), p, std::mem_fn(&Particle::setSecondaryAlphaScrollSpeed));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Emitter Translation", treeFlags))
	{
		addFloatProperty("Emitter-Effect Ratio", p->getEmitterTranslationEffectRatio(), p, std::mem_fn(&Particle::setEmitterTranslationEffectRatio));
		addFloatProperty("Follow Ratio", p->getFollowEmitterTranslationRatio(), p, std::mem_fn(&Particle::setFollowEmitterTranslationRatio));
		addFloatProperty("Follow Y Ratio", p->getFollowEmitterTranslationYRatio(), p, std::mem_fn(&Particle::setFollowEmitterTranslationYRatio));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Blending", treeFlags))
	{
		addComboBoxProperty("Blending Mode", Glitter::blendModeTable, Glitter::blendModeTableSize, p->getBlendMode(),
			p, std::mem_fn(&Particle::setBlendMode));

		addComboBoxProperty("2nd Blending Mode", Glitter::blendModeTable, Glitter::blendModeTableSize, p->getSecondaryBlendMode(),
			p, std::mem_fn(&Particle::setSecondaryBlendMode));

		addFloatProperty("2nd Blend", p->getSecondaryBlend(), p, std::mem_fn(&Particle::setSecondaryBlend));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Flags", defaultOpen))
	{
		addFlagsProperty("Emitter Local", p->getFlags(), 4, p, std::mem_fn(&Particle::setFlags));
		addFlagsProperty("Scale Using X Only", p->getFlags(), 16, p, std::mem_fn(&Particle::setFlags));
		addFlagsProperty("Z Test", p->getFlags(), 64, p, std::mem_fn(&Particle::setFlags));

		addUIntProperty("Flags", p->getFlags(), p, std::mem_fn(&Particle::setFlags));
		ImGui::TreePop();
	}
}

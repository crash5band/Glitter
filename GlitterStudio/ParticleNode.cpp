#include "ParticleNode.h"
#include "UiHelper.h"
#include "File.h"
#include "FileGUI.h"
#include "ResourceManager.h"
#include "GTMManager.h"

namespace Glitter
{
	namespace Editor
	{
		ParticleNode::ParticleNode(std::shared_ptr<Particle>& p) :
			particle{ p }
		{
			animSet = std::make_shared<EditorAnimationSet>(p->getAnimations());
			mesh = nullptr;
		}

		ParticleNode::ParticleNode(std::shared_ptr<ParticleNode>& rhs)
		{
			particle = std::make_shared<Particle>(*rhs->particle);
			animSet = std::make_shared<EditorAnimationSet>(*rhs->animSet);

			mesh = std::shared_ptr<ModelData>(rhs->mesh);
			materialNode = std::shared_ptr<MaterialNode>(rhs->materialNode);
		}

		std::shared_ptr<Particle>& ParticleNode::getParticle()
		{
			return particle;
		}

		std::shared_ptr<MaterialNode>& ParticleNode::getMaterialNode()
		{
			return materialNode;
		}

		std::shared_ptr<ModelData> ParticleNode::getMesh()
		{
			return mesh;
		}

		NodeType ParticleNode::getNodeType()
		{
			return NodeType::Particle;
		}

		std::shared_ptr<EditorAnimationSet> ParticleNode::getAnimationSet()
		{
			return animSet;
		}

		void ParticleNode::setMaterial(std::shared_ptr<MaterialNode>& matNode)
		{
			materialNode = matNode;
			initMesh();
		}

		void ParticleNode::changeMaterial(std::shared_ptr<MaterialNode>& matNode)
		{
			materialNode = matNode;
			particle->setMaterial(File::getFileNameWithoutExtension(matNode->getMaterial()->getFilename()));
		}

		void ParticleNode::changeMesh(std::shared_ptr<ModelData> m)
		{
			mesh = m;
			if (m) particle->setMeshName(File::getFileNameWithoutExtension(m->getName()));
		}

		void ParticleNode::initMesh()
		{
			if (!materialNode)
				return;

			std::string materialPath = File::getFilePath(materialNode->getMaterial()->getFilename());
			if (particle->getType() == ParticleType::Mesh && particle->getName().size())
			{
				ResourceManager::loadModel(materialPath + "\\" + particle->getMeshName() + ".model");
				changeMesh(ResourceManager::getModel(particle->getMeshName() + ".model"));
			}
		}

		void ParticleNode::save()
		{
			particle->getAnimations().clear();
			for (const EditorAnimation& animation : animSet->animations)
				particle->getAnimations().push_back(animation.toGlitterAnimation());

			particle->setMaterial(File::getFileNameWithoutExtension(materialNode->getMaterial()->getFilename()));
		}

		void ParticleNode::populateInspector()
		{
			using Particle = Particle;
			ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			ImGuiTreeNodeFlags defaultOpen = ImGuiTreeNodeFlags_DefaultOpen | treeFlags;

			if (ImGui::TreeNodeEx("Particle", defaultOpen))
			{
				beginPropertyColumn();

				addTextProperty("Name", particle->getName(), particle, std::mem_fn(&Particle::setName));
				addComboBoxProperty("Type", particleTypeTable, particleTypeTableSize, particle->getType(),
					particle, std::mem_fn(&Particle::setType));

				addFloatProperty("Life", particle->getLifeTime(), particle, std::mem_fn(&Particle::setLifeTime));
				addColorProperty("Color", particle->getColor(), particle, std::mem_fn(&Particle::setColor));
				addIntProperty("Max Count", particle->getMaxCount(), particle, std::mem_fn(&Particle::setMaxCount));

				addComboBoxProperty("Pivot", pivotPositionTable, pivotPositionTableSize, particle->getPivotPosition(),
					particle, std::mem_fn(&Particle::setPivotPosition));

				addComboBoxProperty("Direction Type", PdirectionTypeTable, PdirectionTypeTableSize,
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
				beginPropertyColumn();

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

			if (particle->getType() == ParticleType::Mesh)
			{
				if (ImGui::TreeNodeEx("Mesh", defaultOpen))
				{
					beginPropertyColumn();
					ImGui::Text("Mesh");
					ImGui::NextColumn();
					if (ImGui::Button(mesh.get() ? particle->getMeshName().c_str() : "None", ImVec2(ImGui::GetContentRegionAvail().x, UI::btnHeight)))
					{
						std::string name;
						if (FileGUI::openFileGUI(FileType::Model, name))
						{
							ResourceManager::loadModel(name);
							changeMesh(ResourceManager::getModel(File::getFileName(name)));
						}
					}
					ImGui::NextColumn();
					ImGui::TreePop();
				}
				endPropertyColumn();
			}
			else if (particle->getType() == ParticleType::Locus)
			{
				if (ImGui::TreeNodeEx("Locus Properties", defaultOpen))
				{
					beginPropertyColumn();

					addUIntProperty("History Size", particle->getLocusHistorySize(), particle, std::mem_fn(&Particle::setLocusHistorySize));
					addUIntProperty("History Size Random", particle->getLocusHistorySizeRandom(), particle, std::mem_fn(&Particle::setLocusHistorySizeRandom));
					ImGui::TreePop();
				}
				endPropertyColumn();
			}

			if (ImGui::TreeNodeEx("Texture", defaultOpen))
			{
				std::string comboLbl = materialNode ? materialNode->getMaterial()->getName() : std::string(particle->getMaterial() + "(Not Loaded)");
				std::vector<std::shared_ptr<MaterialNode>> materials = GTMManager::getNodes();

				beginPropertyColumn();
				ImGui::Text("Material");
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
				if (ImGui::BeginCombo("##material_list", comboLbl.c_str()))
				{
					for (int n = 0; n < materials.size(); ++n)
					{
						const bool selected = materialNode == materials[n];
						if (ImGui::Selectable(materials[n]->getMaterial()->getName().c_str(), selected))
							changeMaterial(materials[n]);

						if (selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				ImGui::NextColumn();

				addUIntProperty("Texture Index", particle->getTextureIndex(), particle, std::mem_fn(&Particle::setTextureIndex));
				addComboBoxProperty("UV Index Type", uvIndexTypeTable, uvIndexTypeTableSize, particle->getUVIndexType(),
					particle, std::mem_fn(&Particle::setUVIndexType));

				addUIntProperty("UV Index", particle->getUVIndex(), particle, std::mem_fn(&Particle::setUVIndex));
				addFloatProperty("UV Change Interval", particle->getUVChangeInterval(), particle, std::mem_fn(&Particle::setUVChangeInterval));

				ImGui::TreePop();
			}
			endPropertyColumn();

			if (ImGui::TreeNodeEx("Color", treeFlags))
			{
				beginPropertyColumn();

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
				beginPropertyColumn();

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
				beginPropertyColumn();

				addFloatProperty("Emitter Translation Effect Ratio", particle->getEmitterTranslationEffectRatio(), particle, std::mem_fn(&Particle::setEmitterTranslationEffectRatio));
				addFloatProperty("Follow Emitter Translation Ratio", particle->getFollowEmitterTranslationRatio(), particle, std::mem_fn(&Particle::setFollowEmitterTranslationRatio));
				addFloatProperty("Follow Emitter Translation Y Ratio", particle->getFollowEmitterTranslationYRatio(), particle, std::mem_fn(&Particle::setFollowEmitterTranslationYRatio));

				ImGui::TreePop();
			}
			endPropertyColumn();

			if (ImGui::TreeNodeEx("Blending", treeFlags))
			{
				beginPropertyColumn();

				addComboBoxProperty("Blending Mode", blendModeTable, blendModeTableSize, particle->getBlendMode(),
					particle, std::mem_fn(&Particle::setBlendMode));

				addComboBoxProperty("2nd Blending Mode", blendModeTable, blendModeTableSize, particle->getSecondaryBlendMode(),
					particle, std::mem_fn(&Particle::setSecondaryBlendMode));

				addFloatProperty("2nd Blend", particle->getSecondaryBlend(), particle, std::mem_fn(&Particle::setSecondaryBlend));

				ImGui::TreePop();
			}
			endPropertyColumn();

			if (ImGui::TreeNodeEx("Flags", defaultOpen))
			{
				beginPropertyColumn();

				addFlagsProperty("Flag1", particle->getFlags(), 1, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Flag2", particle->getFlags(), 2, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Emitter Local", particle->getFlags(), 4, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Flag8", particle->getFlags(), 8, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Scale Using X Only", particle->getFlags(), 16, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Flag64", particle->getFlags(), 64, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Flag128", particle->getFlags(), 128, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Flag256", particle->getFlags(), 256, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Flag512", particle->getFlags(), 512, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Flag1024", particle->getFlags(), 1024, particle, std::mem_fn(&Particle::setFlags));
				addFlagsProperty("Flag2048", particle->getFlags(), 2048, particle, std::mem_fn(&Particle::setFlags));

				//addUIntProperty("Flags", particle->getFlags(), particle, std::mem_fn(&Particle::setFlags));
				ImGui::TreePop();
			}
			endPropertyColumn();
		}
	}
}

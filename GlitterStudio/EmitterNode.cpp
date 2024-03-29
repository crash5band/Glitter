#include "EmitterNode.h"
#include "EffectNode.h"
#include "Utilities.h"
#include "UiHelper.h"
#include "File.h"
#include "FileDialog.h"
#include "ResourceManager.h"
#include "MathExtensions.h"

namespace Glitter
{
	namespace Editor
	{
		EmitterNode::EmitterNode(std::shared_ptr<Emitter>& em, EffectNode* eff) :
			emitter{ em }, lastEmissionTime{ -1 }, lastRotIncrement{ -1 }, visible{ true }
		{
			animSet = std::make_shared<EditorAnimationSet>(em->getAnimations());

			std::vector<std::weak_ptr<Particle>> emitterParticles = em->getParticles();
			std::vector<std::shared_ptr<ParticleNode>> particleNodes = eff->getParticleNodes();

			for (size_t i = 0; i < emitterParticles.size(); ++i)
			{
				for (size_t j = 0; j < particleNodes.size(); ++j)
				{
					if (particleNodes[j]->getParticle() == emitterParticles[i].lock())
						particleInstances.emplace_back(particleNodes[j]);
				}
			}

			std::string root = File::getFilePath(eff->getEffect()->getFilename());
			if (em->getType() == EmitterType::Mesh && em->getMeshName().size())
			{
				ResourceManager::loadModel(root + "\\" + em->getMeshName() + ".model");
				changeMesh(ResourceManager::getModel(em->getMeshName() + ".model"));
			}

			animSet->markDirty(true);
		}

		EmitterNode::EmitterNode(std::shared_ptr<EmitterNode>& rhs)
		{
			emitter = std::make_shared<Emitter>(*rhs->emitter);
			animSet = std::make_shared<EditorAnimationSet>(*rhs->animSet);

			particleInstances = rhs->particleInstances;
			mesh = std::shared_ptr<ModelData>(rhs->mesh);

			animSet->markDirty(true);
		}

		std::vector<ParticleInstance>& EmitterNode::getParticles()
		{
			return particleInstances;
		}

		std::shared_ptr<Emitter> EmitterNode::getEmitter()
		{
			return emitter;
		}

		std::shared_ptr<EditorAnimationSet> EmitterNode::getAnimationSet()
		{
			return animSet;
		}

		std::shared_ptr<ModelData> EmitterNode::getMesh() const
		{
			return mesh;
		}

		void EmitterNode::changeMesh(std::shared_ptr<ModelData> mesh)
		{
			this->mesh = mesh;
			if (mesh) emitter->setMeshName(File::getFileNameWithoutExtension(mesh->getName()));
		}

		bool EmitterNode::isVisible() const
		{
			return visible;
		}

		void EmitterNode::setVisible(bool val)
		{
			visible = val;
		}

		void EmitterNode::setVisibleAll(bool val)
		{
			visible = val;
			for (auto& p : particleInstances)
				p.setVisible(val);
		}

		void EmitterNode::save()
		{
			emitter->getAnimations().clear();
			for (const EditorAnimation& animation : animSet->animations)
				emitter->getAnimations().push_back(animation.toGlitterAnimation());
		}

		void EmitterNode::emit(float time, int count)
		{
			if (!emissionCount)
				return;

			std::vector<Vector3> basePositions;
			basePositions.reserve(count);

			Vector3 basePos;
			EmissionDirectionType emitDir = EmissionDirectionType::ParticleVelocity;
			DirectX::XMMATRIX m4Origin = mat4;
			Vector3 emTranslation = Vector3(m4Origin.r[3].m128_f32[0], m4Origin.r[3].m128_f32[1], m4Origin.r[3].m128_f32[2]);
			m4Origin.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };

			for (auto& particle : particleInstances)
			{
				for (int i = 0; i < count; ++i)
				{
					if (emitter->getType() == EmitterType::Box)
					{
						Vector3 size = emitter->getSize() / 2.0f;
						basePos = Utilities::randomize(Vector3(), size);
					}
					else if (emitter->getType() == EmitterType::Cylinder)
					{
						float angle = Utilities::toRadians(Utilities::random(emitter->getStartAngle(), emitter->getEndAngle()));
						float height = Utilities::randomize(0, emitter->getHeight() / 2);

						// get x and z points
						float cAngle = cosf(angle);
						float sAngle = sinf(angle);
						float factor = 1.0f / sqrtf(sAngle * sAngle + cAngle * cAngle);
						float x = cAngle * factor;
						float z = sAngle * factor;

						basePos.x = x * emitter->getRadius();
						basePos.y = height;
						basePos.z = z * emitter->getRadius();

						emitDir = emitter->getEmissionDirectionType();
					}
					else if (emitter->getType() == EmitterType::Sphere)
					{
						float longitude = Utilities::random(0.0f, emitter->getLongitude());
						float latitude = Utilities::random(0.0f, emitter->getLatitude());
						longitude = Utilities::toRadians(longitude);
						latitude = Utilities::toRadians(latitude);

						basePos.x = sinf(longitude) * cosf(latitude) * emitter->getRadius();
						basePos.y = sinf(latitude) * emitter->getRadius();
						basePos.z = cosf(longitude) * cosf(latitude) * emitter->getRadius();

						emitDir = emitter->getEmissionDirectionType();
					}
					else if (emitter->getType() == EmitterType::Mesh)
					{
						if (mesh)
						{
							size_t index = Utilities::random(0, mesh->getVertices().size() - 1);
							Vector3 pos = mesh->getVertices()[index].position;
							basePos.x = pos.x;
							basePos.y = pos.y;
							basePos.z = pos.z;
						}
						else
						{
							return;
						}
					}
					else
					{
						return;
					}

					/* 
						include emitter translation in particles' base position if they do not follow the emitter,
						since we do not update the base position if the flag is set.
					*/
					if ((particle.getParticle()->getFlags() & 4) == 0)
					{
						DirectX::XMVECTOR bp{ basePos.x, basePos.y, basePos.z };
						bp = DirectX::XMVector3Transform(bp, m4Origin);
						basePos = Vector3(bp.m128_f32[0], bp.m128_f32[1], bp.m128_f32[2]) + emTranslation;
					}

					basePositions.emplace_back(basePos);
				}

				particle.create(count, time, emitDir, basePositions);
			}
		}

		void EmitterNode::updateMatrix(const Vector3& pos, const Quaternion& rot, const Vector3& scale,
			const Camera& cam, const DirectX::XMMATRIX &effMat)
		{
			mat4 = DirectX::XMMatrixIdentity();
			mat4 *= DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			mat4 *= DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{ rot.x, rot.y, rot.z, rot.w });

			DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
			bool mulViewM4 = true;

			switch (emitter->getDirectionType())
			{
			case EmitterDirectionType::Billboard:
				view = cam.getViewMatrix();
				view.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
				view = DirectX::XMMatrixInverse(nullptr, view);
				view.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
				break;

			case EmitterDirectionType::XAxis:
				view = DirectX::XMMatrixRotationY(-PI / 2);
				break;

			case EmitterDirectionType::YAxis:
				view = DirectX::XMMatrixRotationX(PI / 2);
				break;

			case EmitterDirectionType::ZAxis:
				view = DirectX::XMMatrixRotationZ(-PI / 2);
				break;

			case EmitterDirectionType::YRotationOnly:
				view = DirectX::XMMatrixRotationY(PI);
				view *= DirectX::XMMatrixRotationY(Utilities::toRadians(90 - cam.getYaw()));
				break;

			default:
				mulViewM4 = false;
				break;
			}

			if (mulViewM4)
				mat4 = mat4 * view;

			// rotate emitter around effect
			DirectX::XMMATRIX effM4Origin = effMat;
			effM4Origin.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };

			DirectX::XMVECTOR emPos{ pos.x, pos.y, pos.z };
			emPos = DirectX::XMVector3Transform(emPos, effM4Origin);
			mat4 *= DirectX::XMMatrixTranslationFromVector(emPos);
		}

		void EmitterNode::update(float time, float effTime, const Camera& camera, const DirectX::XMMATRIX &effM4, const Quaternion &effRot)
		{
			float emitterTime = time - emitter->getStartTime();
			float emitterLife = fmodf(emitterTime, emitter->getLifeTime() + 1);
			float emissionTime = round(emitterTime);

			if ((int)effTime == 0)
			{
				rotationAdd = Vector3();
				lastRotIncrement = (int)emitterTime;
			}
			else if ((int)emitterLife == 0 && ((int)lastRotIncrement != (int)emitterTime))
			{
				rotationAdd += Utilities::randomize(emitter->getRotationAdd(), emitter->getRotationAddRandom());
				lastRotIncrement = (int)emitterTime;
			}

			if (animSet->isDirty())
			{
				animationCache.buildCache(animSet);
				animSet->markDirty(false);
			}

			Quaternion qR;
			if (emitterLife >= 0.0f)
			{
				Vector3 effTranslation = Vector3(effM4.r[3].m128_f32[0], effM4.r[3].m128_f32[1], effM4.r[3].m128_f32[2]);
				Vector3 translation = emitter->getTranslation() + animationCache.tryGetTranslation(emitterLife) + effTranslation;
				Vector3 rotation = emitter->getRotation() + rotationAdd + animationCache.tryGetRotation(emitterLife);
				Vector3 scale = emitter->getScaling() *= animationCache.tryGetScale(emitterLife);
				qR = effRot * MathExtensions::fromRotationZYX(rotation);

				updateMatrix(translation, qR, scale, camera, effM4);

				emissionCount = emitter->getParticlesPerEmission();
				int count = round(animationCache.getValue(AnimationType::ParticlePerEmission, emitterLife, -1));
				if (count > -1)
					emissionCount = count;

				// return -1 if no interval animation is applied since the animation can have a value of 0.
				emissionInterval = emitter->getEmissionInterval();
				float interval = animationCache.getValue(AnimationType::EmissionInterval, emitterLife, -1.0f);
				if (interval > -1.0f)
					emissionInterval = interval;

				if (((emissionTime <= emitter->getLifeTime()) || (emitter->getFlags() & 1)) && emissionInterval > 0.0f)
				{
					if (emitter->getEmitCondition() == EmitCondition::Time)
					{
						if ((fmodf(emissionTime, emissionInterval) <= 0.1f || emissionTime == 0) && ((int)emissionTime != (int)lastEmissionTime))
						{
							lastEmissionTime = (int)emissionTime;
							emit(emitterTime, emissionCount);
						}
					}
					else
					{
						float delta = translation.distance(lastEmissionPosition);
						if ((fmodf(delta, emissionInterval) <= 0.1f) && (lastEmissionPosition != translation))
						{
							emit(emitterTime, emissionCount);
							lastEmissionPosition = translation;
						}
					}
				}
			}

			for (auto& particle : particleInstances)
				particle.update(emitterTime, camera, mat4, qR);
		}

		void EmitterNode::kill()
		{
			for (auto& particle : particleInstances)
				particle.kill();
		}

		NodeType EmitterNode::getNodeType()
		{
			return NodeType::Emitter;
		}

		void EmitterNode::populateInspector()
		{
			using Emitter = Emitter;
			ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

			if (ImGui::TreeNodeEx("Emitter", treeFlags))
			{
				beginPropertyColumn();

				addTextProperty("Name", emitter->getName(), emitter, std::mem_fn(&Emitter::setName));
				addComboBoxProperty("Type", emitterTypeTable, emitterTypeTableSize,
					emitter->getType(), emitter, std::mem_fn(&Emitter::setType));

				addFloatProperty("Start", emitter->getStartTime(), emitter, std::mem_fn(&Emitter::setStartTime));
				addFloatProperty("Life", emitter->getLifeTime(), emitter, std::mem_fn(&Emitter::setLifeTime));
				addFloatProperty("Loop Start", emitter->getLoopStartTime(), emitter, std::mem_fn(&Emitter::setLoopStartTime));
				addFloatProperty("Loop End", emitter->getLoopEndTime(), emitter, std::mem_fn(&Emitter::setLoopEndTime));

				addVector3Property("Translation", emitter->getTranslation(), emitter, std::mem_fn(&Emitter::setTranslation));
				addVector3Property("Rotation", emitter->getRotation(), emitter, std::mem_fn(&Emitter::setRotation));
				addVector3Property("Rotation Add", emitter->getRotationAdd(), emitter, std::mem_fn(&Emitter::setRotationAdd));
				addVector3Property("Rotation Add Random", emitter->getRotationAddRandom(), emitter, std::mem_fn(&Emitter::setRotationAddRandom));
				addVector3Property("Scaling", emitter->getScaling(), emitter, std::mem_fn(&Emitter::setScaling));

				ImGui::TreePop();
			}
			endPropertyColumn();

			if (ImGui::TreeNodeEx("Emission", treeFlags))
			{
				beginPropertyColumn();

				addComboBoxProperty("Condition", emitConditionTable, emitConditionTableSize,
					emitter->getEmitCondition(), emitter, std::mem_fn(&Emitter::setEmitCondition));

				addComboBoxProperty("Direction Type", emitterDirectionTypeTable, emitterDirectionTypeTableSize,
					emitter->getDirectionType(), emitter, std::mem_fn(&Emitter::setDirectionType));

				addFloatProperty("Interval", emitter->getEmissionInterval(), emitter, std::mem_fn(&Emitter::setEmissionInterval));
				addIntProperty("Particles Per Emission", emitter->getParticlesPerEmission(), emitter, std::mem_fn(&Emitter::setParticlePerEmission));

				ImGui::TreePop();
			}
			endPropertyColumn();

			// Type specific properties
			switch (emitter->getType())
			{
			case EmitterType::Box:
				if (ImGui::TreeNodeEx("Box Properties", treeFlags))
				{
					beginPropertyColumn();

					addVector3Property("Size", emitter->getSize(), emitter, std::mem_fn(&Emitter::setSize));
					ImGui::TreePop();
				}
				break;

			case EmitterType::Cylinder:
				if (ImGui::TreeNodeEx("Cylinder Properties", treeFlags))
				{
					beginPropertyColumn();

					addFloatProperty("Radius", emitter->getRadius(), emitter, std::mem_fn(&Emitter::setRadius));
					addFloatProperty("Height", emitter->getHeight(), emitter, std::mem_fn(&Emitter::setHeight));
					addFloatProperty("Start Angle", emitter->getStartAngle(), emitter, std::mem_fn(&Emitter::setStartAngle));
					addFloatProperty("End Angle", emitter->getEndAngle(), emitter, std::mem_fn(&Emitter::setEndAngle));
					addComboBoxProperty("Direction Type", emissionDirectionTypeTable, emissionDirectionTypeTableSize,
						emitter->getEmissionDirectionType(), emitter, std::mem_fn(&Emitter::setEmissionDirectionType));

					ImGui::TreePop();
				}
				break;

			case EmitterType::Sphere:
				if (ImGui::TreeNodeEx("Sphere Properties", treeFlags))
				{
					beginPropertyColumn();

					addFloatProperty("Radius", emitter->getRadius(), emitter, std::mem_fn(&Emitter::setRadius));
					addFloatProperty("Latitude", emitter->getLatitude(), emitter, std::mem_fn(&Emitter::setLatitude));
					addFloatProperty("Longitude", emitter->getLongitude(), emitter, std::mem_fn(&Emitter::setLongitude));
					addComboBoxProperty("Emission Direction Type", emissionDirectionTypeTable, emissionDirectionTypeTableSize,
						emitter->getEmissionDirectionType(), emitter, std::mem_fn(&Emitter::setEmissionDirectionType));

					ImGui::TreePop();
				}
				break;

			case EmitterType::Mesh:
				if (ImGui::TreeNodeEx("Mesh", treeFlags))
				{
					beginPropertyColumn();

					ImGui::Text("Mesh");
					ImGui::NextColumn();
					if (ImGui::Button(mesh.get() ? emitter->getMeshName().c_str() : "None", ImVec2(ImGui::GetContentRegionAvail().x, UI::btnHeight)))
					{
						std::string name;
						if (FileDialog::openFileDialog(FileType::Model, name))
						{
							ResourceManager::loadModel(name);
							changeMesh(ResourceManager::getModel(File::getFileName(name)));
						}
					}
					ImGui::NextColumn();
					ImGui::TreePop();
				}
				break;

			case EmitterType::Polygon:
				if (ImGui::TreeNodeEx("Polygon Properties", treeFlags))
				{
					beginPropertyColumn();

					addIntProperty("Point Count", emitter->getPointCount(), emitter, std::mem_fn(&Emitter::setPointCount));
					ImGui::TreePop();
				}
				break;
			}
			endPropertyColumn();

			if (ImGui::TreeNodeEx("Flags", treeFlags))
			{
				beginPropertyColumn();

				addFlagsProperty("Loop", emitter->getFlags(), 1, emitter, std::mem_fn(&Emitter::setFlags));
				ImGui::TreePop();
			}
			endPropertyColumn();
		}
	}
}

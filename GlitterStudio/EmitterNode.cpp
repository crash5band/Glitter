#include "EmitterNode.h"
#include "EffectNode.h"
#include "Utilities.h"
#include "UiHelper.h"
#include "File.h"
#include "FileGUI.h"
#include "ResourceManager.h"

EmitterNode::EmitterNode(std::shared_ptr<Glitter::Emitter> &em, EffectNode* eff) :
	emitter{ em }, lastEmissionTime{ -1 }, visible{ true }
{
	animationNode = std::make_shared<AnimationNode>(&em->getAnimations());

	std::vector<std::weak_ptr<Glitter::Particle>> emitterParticles = em->getParticles();
	std::vector<std::shared_ptr<ParticleNode>> particleNodes = eff->getParticleNodes();

	for (size_t i = 0; i < emitterParticles.size(); ++i)
	{
		for (size_t j = 0; j < particleNodes.size(); ++j)
		{
			if (particleNodes[j]->getParticle() == emitterParticles[i].lock())
				particleInstances.emplace_back(particleNodes[j]);
		}
	}
}

std::vector<ParticleInstance> &EmitterNode::getParticles()
{
	return particleInstances;
}

std::shared_ptr<Glitter::Emitter> EmitterNode::getEmitter()
{
	return emitter;
}

std::shared_ptr<AnimationNode> EmitterNode::getAnimationNode()
{
	return animationNode;
}

void EmitterNode::changeMesh(const std::string& filename)
{
	ResourceManager::loadModel(filename);
	mesh = ResourceManager::getModel(Glitter::File::getFileName(filename));
	emitter->setMeshName(Glitter::File::getFileNameWithoutExtension(filename));
}

Glitter::Vector3 EmitterNode::getPosition(float time)
{
	return emitter->getTranslation() + animationNode->tryGetTranslation(time);
}

Glitter::Vector3 EmitterNode::getRotation(float time)
{
	// TODO: Account for RotationAdd and RotationAddRandom
	return emitter->getRotation() + animationNode->tryGetRotation(time);
}

Glitter::Vector3 EmitterNode::getScaling(float time)
{
	return emitter->getScaling() *= animationNode->tryGetScale(time);
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

void EmitterNode::emit(float time, Transform& baseTransform)
{
	if (!emissionCount)
		return;

	std::vector<Glitter::Vector3> basePositions;
	Glitter::Vector3 basePos;

	Glitter::Matrix3 m3;
	Glitter::Matrix4 m4;

	Glitter::EmissionDirectionType emitDir = Glitter::EmissionDirectionType::ParticleVelocity;
	Glitter::EmitterDirectionType dirType = emitter->getDirectionType();

	float rotX = Utilities::toRadians(transform.rotation.x);
	float rotY = Utilities::toRadians(transform.rotation.y);
	float rotZ = Utilities::toRadians(transform.rotation.z);

	m3.fromEulerAnglesZYX(rotZ, rotY, rotX);
	m4.makeTransform(transform.position, transform.scale, m3);

	for (auto& particle : particleInstances)
	{
		for (int i = 0; i < emissionCount; ++i)
		{
			if (emitter->getType() == Glitter::EmitterType::Box)
			{
				Glitter::Vector3 size = emitter->getSize() / 2.0f;
				basePos = Utilities::randomize(Glitter::Vector3(), size);
			}
			else if (emitter->getType() == Glitter::EmitterType::Cylinder)
			{
				float angle = Utilities::random(emitter->getStartAngle(), emitter->getEndAngle());
				angle = Utilities::toRadians(angle);

				float height = emitter->getHeight() / 2;
				float randomHeight = Utilities::random(-height, height);

				basePos.x = cos(angle) * emitter->getRadius();
				basePos.y = randomHeight;
				basePos.z = sin(angle) * emitter->getRadius();

				emitDir = emitter->getEmissionDirectionType();
			}
			else if (emitter->getType() == Glitter::EmitterType::Sphere)
			{
				float longitude = Utilities::random(0.0f, emitter->getLongitude());
				float latitude = Utilities::random(0.0f, emitter->getLatitude());
				longitude = Utilities::toRadians(longitude);
				latitude = Utilities::toRadians(latitude);

				basePos.x = cos(longitude) * cos(latitude) * emitter->getRadius();
				basePos.y = sin(latitude) * emitter->getRadius();
				basePos.z = sin(longitude) * cos(latitude) * emitter->getRadius();

				emitDir = emitter->getEmissionDirectionType();
			}
			else if (emitter->getType() == Glitter::EmitterType::Mesh)
			{
				if (mesh)
				{
					size_t index = Utilities::random(0, mesh->getVertices().size() - 1);
					Glitter::Vector3 pos = mesh->getVertices()[index].position;
					basePos = transform.position;
					basePos.x += pos.x;
					basePos.y += pos.y;
					basePos.z += pos.z;
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

			basePos = m4 * basePos;
			basePositions.emplace_back(basePos);
		}

		particle.create(emissionCount, time, emitDir, basePositions, transform.position);
	}
}

void EmitterNode::changeDirection(Glitter::EmitterDirectionType type, Camera* camera)
{
	switch (type)
	{
	case Glitter::EmitterDirectionType::Billboard:
		transform.rotation.x = transform.rotation.x - camera->getPitch();
		transform.rotation.y = 90.0f - camera->getYaw();
		break;

	case Glitter::EmitterDirectionType::YRotationOnly:
		transform.rotation.y = 90.0f - camera->getYaw();
		break;

	case Glitter::EmitterDirectionType::XAxis:
		transform.rotation.y = 90.0f;
		transform.rotation.z = 0.0f;
		break;

	case Glitter::EmitterDirectionType::YAxis:
		transform.rotation.x = -90.0f;
		transform.rotation.z = 0.0f;
		break;

	case Glitter::EmitterDirectionType::ZAxis:
		transform.rotation.x = 0.0f;
		transform.rotation.y = 0.0f;
		break;
	}
}

void EmitterNode::update(float time, Camera* camera, Transform& baseTransform)
{
	float emitterTime = time - emitter->getStartTime();
	float emissionTime = round(emitterTime);

	if (emitterTime >= 0.0f)
	{
		transform = baseTransform;
		transform.position += getPosition(emitterTime);
		transform.rotation += getRotation(emitterTime);
		transform.rotation -= baseTransform.rotation;
		transform.scale = getScaling(emitterTime);
		changeDirection(emitter->getDirectionType(), camera);

		emissionCount = emitter->getParticlesPerEmission();
		int count = animationNode->tryGetValue(Glitter::AnimationType::ParticlePerEmission, time, -1);
		if (count > -1)
			emissionCount = count;

		emissionInterval = emitter->getEmissionInterval();
		float interval = animationNode->tryGetValue(Glitter::AnimationType::EmissionInterval, time, -1.0f);
		if (interval > -1.0f)
			emissionInterval = interval;

		emissionInterval = round(emissionInterval);
		if (((emissionTime <= emitter->getLifeTime()) || (emitter->getFlags() & 1)) && emissionInterval > 0.5f)
		{
			if (emitter->getEmitCondition() == Glitter::EmitCondition::Time)
			{
				if (((fmodf(emissionTime, emissionInterval) == 0.0f) || emissionTime == 0) && (emissionTime != lastEmissionTime))
				{
					lastEmissionTime = emissionTime;
					emit(emitterTime, baseTransform);
				}
			}
			else
			{
				float delta = transform.position.distance(lastEmissionPosition);
				if ((fmodf(delta, interval) <= 0.1f) && (lastEmissionPosition != transform.position))
				{
					emit(emitterTime, baseTransform);
					lastEmissionPosition = transform.position;
				}
			}
		}
	}

	Transform t = baseTransform;
	t.scale = transform.scale;

	if (emitter->getType() == Glitter::EmitterType::Box)
	{
		t.rotation += transform.rotation;
	}

	for (auto& particle : particleInstances)
	{
		particle.update(emitterTime, camera, t, transform);
	}
}

void EmitterNode::kill()
{
	for (auto& particle : particleInstances)
		particle.kill();
}

NodeType EmitterNode::getType()
{
	return NodeType::Emitter;
}

void EmitterNode::populateInspector()
{
	using Emitter = Glitter::Emitter;
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

	if (ImGui::TreeNodeEx("Emitter", treeFlags))
	{
		addTextProperty("Name", emitter->getName(), emitter, std::mem_fn(&Emitter::setName));

		addComboBoxProperty("Type", Glitter::emitterTypeTable, Glitter::emitterTypeTableSize,
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
		addComboBoxProperty("Condition", Glitter::emitConditionTable, Glitter::emitConditionTableSize,
			emitter->getEmitCondition(), emitter, std::mem_fn(&Emitter::setEmitCondition));

		addComboBoxProperty("Direction Type", Glitter::EdirectionTypeTable, Glitter::EdirectionTypeTableSize,
			emitter->getDirectionType(), emitter, std::mem_fn(&Emitter::setDirectionType));

		addFloatProperty("Interval", emitter->getEmissionInterval(), emitter, std::mem_fn(&Emitter::setEmissionInterval));
		addIntProperty("Particles Per Emission", emitter->getParticlesPerEmission(), emitter, std::mem_fn(&Emitter::setParticlePerEmission));

		ImGui::TreePop();
	}
	endPropertyColumn();

	// Type specific properties
	switch (emitter->getType())
	{
	case Glitter::EmitterType::Box:
		if (ImGui::TreeNodeEx("Box Properties", treeFlags))
		{
			addVector3Property("Size", emitter->getSize(), emitter, std::mem_fn(&Emitter::setSize));
			ImGui::TreePop();
		}
		break;

	case Glitter::EmitterType::Cylinder:
		if (ImGui::TreeNodeEx("Cylinder Properties", treeFlags))
		{
			addFloatProperty("Radius", emitter->getRadius(), emitter, std::mem_fn(&Emitter::setRadius));
			addFloatProperty("Height", emitter->getHeight(), emitter, std::mem_fn(&Emitter::setHeight));
			addFloatProperty("Start Angle", emitter->getStartAngle(), emitter, std::mem_fn(&Emitter::setStartAngle));
			addFloatProperty("End Angle",	emitter->getEndAngle(), emitter, std::mem_fn(&Emitter::setEndAngle));
			addComboBoxProperty("Direction Type", Glitter::emissionDirectionTypeTable, Glitter::emissionDirectionTypeTableSize,
				emitter->getEmissionDirectionType(), emitter, std::mem_fn(&Emitter::setEmissionDirectionType));

			ImGui::TreePop();
		}
		break;

	case Glitter::EmitterType::Sphere:
		if (ImGui::TreeNodeEx("Sphere Properties", treeFlags))
		{
			addFloatProperty("Radius", emitter->getRadius(), emitter, std::mem_fn(&Emitter::setRadius));
			addFloatProperty("Latitude", emitter->getLatitude(), emitter, std::mem_fn(&Emitter::setLatitude));
			addFloatProperty("Longitude", emitter->getLongitude(), emitter, std::mem_fn(&Emitter::setLongitude));
			addComboBoxProperty("Emission Direction Type", Glitter::emissionDirectionTypeTable, Glitter::emissionDirectionTypeTableSize,
				emitter->getEmissionDirectionType(), emitter, std::mem_fn(&Emitter::setEmissionDirectionType));

			ImGui::TreePop();
		}
		break;

	case Glitter::EmitterType::Mesh:
		if (ImGui::TreeNodeEx("Mesh", treeFlags))
		{
			beginPropertyColumn("Mesh");
			if (ImGui::Button(mesh.get() ? emitter->getMeshName().c_str() : "None", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight())))
			{
				std::string name;
				if (FileGUI::openFileGUI(FileType::Model, name))
					changeMesh(name);
			}
			ImGui::NextColumn();
			ImGui::TreePop();
		}
		break;

	case Glitter::EmitterType::Polygon:
		if (ImGui::TreeNodeEx("Polygon Properties", treeFlags))
		{
			addIntProperty("Point Count", emitter->getPointCount(), emitter, std::mem_fn(&Emitter::setPointCount));
			ImGui::TreePop();
		}
		break;
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Flags", treeFlags))
	{
		addFlagsProperty("Loop", emitter->getFlags(), 1, emitter, std::mem_fn(&Emitter::setFlags));
		addUIntProperty("Flags", emitter->getFlags(), emitter, std::mem_fn(&Emitter::setFlags));
		ImGui::TreePop();
	}
	endPropertyColumn();
}

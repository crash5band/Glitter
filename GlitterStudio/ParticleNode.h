#pragma once
#include "INode.h"
#include "Particle.h"
#include "MaterialNode.h"
#include "ModelData.h"

namespace Glitter
{
	namespace Editor
	{
		class ParticleNode : public INode
		{
		private:
			std::shared_ptr<Particle> particle;
			std::shared_ptr<EditorAnimationSet> animSet;
			std::shared_ptr<MaterialNode> materialNode;
			std::shared_ptr<ModelData> mesh;

		public:
			ParticleNode(std::shared_ptr<Particle>& p);
			ParticleNode(std::shared_ptr<ParticleNode>& rhs);

			std::shared_ptr<Particle>& getParticle();
			std::shared_ptr<MaterialNode>& getMaterialNode();
			std::shared_ptr<ModelData> getMesh();
			void changeMesh(std::shared_ptr<ModelData> mesh);
			void setMaterial(std::shared_ptr<MaterialNode>& matNode);
			void changeMaterial(std::shared_ptr<MaterialNode>& matNode);
			void initMesh();
			void saveAnimations();

			virtual NodeType getNodeType() override;
			virtual void populateInspector() override;
			virtual std::shared_ptr<EditorAnimationSet> getAnimationSet() override;
		};
	}
}

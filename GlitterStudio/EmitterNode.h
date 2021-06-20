#pragma once
#include "INode.h"
#include "ParticleNode.h"
#include "Transform.h"
#include "Emitter.h"
#include "ParticleInstance.h"
#include "ModelData.h"
#include "CachedAnimation.h"

namespace Glitter
{
	namespace Editor
	{
		class EffectNode;

		class EmitterNode : public INode
		{
		private:
			std::shared_ptr<Emitter> emitter;
			std::shared_ptr<EditorAnimationSet> animSet;
			DirectX::XMMATRIX mat4;
			CachedAnimation animationCache;
			std::shared_ptr<ModelData> mesh;
			std::vector<ParticleInstance> particleInstances;

			bool visible;
			int emissionCount;
			float emissionInterval;
			float lastEmissionTime;
			float lastRotIncrement;
			Vector3 lastEmissionPosition;
			Vector3 rotationAdd;

		public:
			EmitterNode(std::shared_ptr<Emitter>& em, EffectNode* parent);
			EmitterNode(std::shared_ptr<EmitterNode>& rhs);

			std::shared_ptr<Emitter> getEmitter();
			std::vector<ParticleInstance>& getParticles();

			virtual NodeType getNodeType() override;
			virtual void populateInspector() override;
			virtual std::shared_ptr<EditorAnimationSet> getAnimationSet() override;

			void update(float time, float effTime, const Camera& camera, const DirectX::XMMATRIX &effM4, const Quaternion &effRot);
			void emit(float time, int count);
			void kill();
			void changeMesh(const std::string& filename);
			void saveAnimations();
			void setVisible(bool val);
			void setVisibleAll(bool val);
			bool isVisible() const;
		};

	}
}

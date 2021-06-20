#pragma once
#include "INode.h"
#include "EmitterNode.h"
#include "ParticleNode.h"
#include "GlitterEffect.h"

namespace Glitter
{
	namespace Editor
	{
		class EffectNode : public INode
		{
		private:
			std::shared_ptr<GlitterEffect> effect;
			std::shared_ptr<EditorAnimationSet> animSet;
			std::vector<std::shared_ptr<EmitterNode>> emitterNodes;
			std::vector<std::shared_ptr<ParticleNode>> particleNodes;
			DirectX::XMMATRIX mat4;
			CachedAnimation animationCache;

		public:
			EffectNode(std::shared_ptr<GlitterEffect>& eff);

			std::shared_ptr<GlitterEffect> getEffect();
			std::vector<std::shared_ptr<EmitterNode>>& getEmitterNodes();
			std::vector<std::shared_ptr<ParticleNode>>& getParticleNodes();

			void update(float time, const Camera& camera);
			void kill();
			void save(const std::string& filename);

			virtual NodeType getNodeType() override;
			virtual void populateInspector() override;
			virtual std::shared_ptr<EditorAnimationSet> getAnimationSet() override;
		};
	}
}



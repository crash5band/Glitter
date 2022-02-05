#pragma once
#include "ParticleNode.h"
#include "CachedAnimation.h"
#include "Camera.h"

namespace Glitter
{
	namespace Editor
	{
		struct LocusHistory
		{
			Vector3 pos;
			Vector3 scale;
			Color color;
		};

		struct ParticleStatus
		{
			DirectX::XMMATRIX mat4;
			Vector3 basePos;
			Vector3 direction;
			Vector3 acceleration;
			Vector3 rotation;
			Vector3 scale;
			Color color;
			Vector2 uvScroll;
			int UVIndex;
			float lastUVChange;
			float startTime;
			float time;
			bool dead;
			std::vector<LocusHistory> locusHistories;
			CachedAnimation animation;

			ParticleStatus() : dead{ true }
			{
			}
		};

		class ParticleInstance
		{
		private:
			std::shared_ptr<ParticleNode> reference;
			std::vector<ParticleStatus> pool;
			size_t rotationAddCount;
			size_t aliveCount;
			bool visible;

			void verifyPoolSize();
			void updateLocusHistory(ParticleStatus& p);

		public:
			ParticleInstance(std::weak_ptr<ParticleNode> ref);

			void update(float time, const Camera& camera, const DirectX::XMMATRIX& emM4, const Quaternion& emRot);
			void create(int count, float startTime, EmissionDirectionType dir, const std::vector<Vector3>& pos);
			void kill();
			void setVisible(bool val);
			bool isVisible() const;
			Vector3 getAnchorPoint(PivotPosition pivot);

			std::vector<ParticleStatus>& getPool();
			std::shared_ptr<ParticleNode> getReference() const;
			std::shared_ptr<Particle> getParticle() const;
			size_t getAliveCount() const;
		};
	}
}

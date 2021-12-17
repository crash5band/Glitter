#include "ParticleInstance.h"
#include "Utilities.h"

namespace Glitter
{
	namespace Editor
	{
		ParticleInstance::ParticleInstance(std::weak_ptr<ParticleNode> nodeRef) :
			reference{ nodeRef }, aliveCount{ 0 }, rotationAddCount{ 0 }, visible{ true }
		{
			size_t count = reference->getParticle()->getMaxCount();
			pool.reserve(count);
			for (size_t i = 0; i < count; ++i)
				pool.emplace_back();
		}

		std::vector<ParticleStatus>& ParticleInstance::getPool()
		{
			return pool;
		}

		std::shared_ptr<ParticleNode> ParticleInstance::getReference() const
		{
			return reference;
		}

		std::shared_ptr<Particle> ParticleInstance::getParticle() const
		{
			return reference->getParticle();
		}

		size_t ParticleInstance::getAliveCount() const
		{
			return aliveCount;
		}

		void ParticleInstance::verifyPoolSize()
		{
			// resize pool if MaxCount is changed
			auto& particle = reference->getParticle();
			size_t size = pool.size();

			if (particle->getMaxCount() > size)
			{
				pool.reserve(particle->getMaxCount());

				for (size_t i = 0; i < particle->getMaxCount() - size; ++i)
					pool.emplace_back();
			}
			else if (particle->getMaxCount() < pool.size())
			{
				for (size_t i = 0; i < size - particle->getMaxCount(); ++i)
				{
					pool.pop_back();
				}

				//pool.shrink_to_fit();
			}
		}

		void ParticleInstance::kill()
		{
			for (auto& p : pool)
			{
				p.locusHistories.clear();
				p.dead = true;
			}
		}

		void ParticleInstance::setVisible(bool val)
		{
			visible = val;
		}

		bool ParticleInstance::isVisible() const
		{
			return visible;
		}

		Vector3 ParticleInstance::getAnchorPoint(PivotPosition pivot)
		{
			switch (pivot)
			{
			case PivotPosition::TopLeft:
				return Vector3(0.5, -0.5, 0);

			case PivotPosition::TopCenter:
				return Vector3(0, -0.5, 0);

			case PivotPosition::TopRight:
				return Vector3(-0.5, -0.5, 0);

			case PivotPosition::MiddleLeft:
				return Vector3(0.5, 0, 0);

			case PivotPosition::MiddleRight:
				return Vector3(-0.5, 0, 0);

			case PivotPosition::BottomLeft:
				return Vector3(0.5, 0.5, 0);

			case PivotPosition::BottomCenter:
				return Vector3(0, 0.5, 0);

			case PivotPosition::BottomRight:
				return Vector3(-0.5, 0.5, 0);

			default:
				return Vector3(0, 0, 0);
			}
		}

		void ParticleInstance::update(float time, const Camera& camera, const DirectX::XMMATRIX &emM4, const Quaternion &emRot)
		{
			verifyPoolSize();

			auto& particle = reference->getParticle();

			// calculate UV params.
			unsigned int maxUV = 0;
			int interval = particle->getUVChangeInterval();
			UVIndexType type = particle->getUVIndexType();
			if (reference->getMaterialNode())
			{
				Vector2 uvSplit = reference->getMaterialNode()->getMaterial()->getSplit();
				maxUV = ((int)uvSplit.x * (int)uvSplit.y) - 1;
			}

			DirectX::XMMATRIX emM4Origin = emM4;
			emM4Origin.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };

			std::shared_ptr<EditorAnimationSet> animationSet = reference->getAnimationSet();

			DirectX::XMMATRIX directionM4 = DirectX::XMMatrixIdentity();
			DirectX::XMMATRIX inverseViewM4 = DirectX::XMMatrixIdentity();
			//inverseViewM4 = DirectX::XMMatrixRotationY(PI);
			inverseViewM4.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
			inverseViewM4 *= DirectX::XMMatrixInverse(nullptr, camera.getViewMatrix());
			inverseViewM4.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };

			ParticleDirectionType dType = particle->getDirectionType();

			aliveCount = 0;
			for (auto& p : pool)
			{
				if (p.time > particle->getLifeTime() || p.time < 0.0f)
					p.dead = true;

				if (p.dead)
					continue;

				if (animationSet->isDirty())
					p.animation.buildCache(animationSet);

				float lastTime = p.time;
				p.time = time - p.startTime;

				Vector3 velocity = p.direction + (p.acceleration * p.time);
				Vector3 basePos = p.basePos;
				Vector3 animT = p.animation.tryGetTranslation(p.time);
				Vector3 gravity = (particle->getGravitationalAccel() / 3600) * p.time * p.time;

				if (particle->getFlags() & 4)
				{
					// FLAGS: Emitter Local
					// transform particle local to emitter axis.
					DirectX::XMVECTOR pTransform{ basePos.x, basePos.y, + basePos.z };
					pTransform = DirectX::XMVector3Transform(pTransform, emM4Origin);
					pTransform = DirectX::XMVectorAdd(pTransform, emM4.r[3]);
					basePos = Vector3(pTransform.m128_f32[0], pTransform.m128_f32[1], pTransform.m128_f32[2]);

					DirectX::XMVECTOR vTransform{ velocity.x, velocity.y, velocity.z };
					vTransform = DirectX::XMVector3Transform(vTransform, emM4Origin);
					velocity = Vector3(vTransform.m128_f32[0], vTransform.m128_f32[1], vTransform.m128_f32[2]);
				}

				// animations not included in emitter local transform
				DirectX::XMVECTOR aTransform{ animT.x, animT.y, animT.z };
				aTransform = DirectX::XMVector3Transform(aTransform, emM4Origin);
				animT = Vector3(aTransform.m128_f32[0], aTransform.m128_f32[1], aTransform.m128_f32[2]);

				Vector3 translation = basePos + (velocity * p.time) + animT + gravity;
				Vector3 rotation = p.rotation + p.animation.tryGetRotation(p.time);
				Vector3 scaling = p.animation.tryGetScale(p.time);

				// mesh particles are only scaled using animations
				if (particle->getType() != ParticleType::Mesh)
				{
					// FLAGS: Uniform Scale
					if (particle->getFlags() & 16)
						scaling *= p.scale.x;
					else
						scaling *= p.scale;
				}

				Vector3 pivot = getAnchorPoint(particle->getPivotPosition());
				pivot *= scaling;

				p.mat4 = DirectX::XMMatrixIdentity();
				p.mat4 *= DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);
				p.mat4 *= DirectX::XMMatrixTranslation(pivot.x, pivot.y, pivot.z);

				switch (dType)
				{
				case ParticleDirectionType::Billboard:
					directionM4 = inverseViewM4;
					break;

				case ParticleDirectionType::DirectionalAngle:
				case ParticleDirectionType::DirectionalAngleBillboard:
				{
					Vector3 diff = translation - basePos;
					float length = diff.squaredLength();

					if (length < 0.000001f)
						diff = basePos;

					diff.normalise();
					directionM4 = DirectX::XMMatrixIdentity();

					Vector3 up(0.0f, 1.0f, 0.0f);

					float t;
					Vector3 axis = up.crossProduct(diff);
					float angle = axis.length();

					if (angle >= 0.000001f)
					{
						angle = asinf(std::min(angle, 1.0f));
					}
					else
					{
						angle = 0.0f;
						axis.x = up.z;
						axis.y = 0.0f;
						axis.z = up.x;
						t = axis.length();
						if (t < 0.000001f)
						{
							axis.x = -up.y;
							axis.y = up.x;
							axis.z = 0.0f;
						}
					}

					t = up.dotProduct(diff);
					if (t < 0.0f)
						angle = PI - angle;

					if (dType == ParticleDirectionType::DirectionalAngleBillboard)
					{
						//directionM4 *= DirectX::XMMatrixRotationY(PI);
						directionM4.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
						directionM4 *= DirectX::XMMatrixRotationY(Utilities::toRadians(90 - camera.getYaw()));
						directionM4.r[3] = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
					}

					directionM4 *= DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ axis.x, axis.y, axis.z, 1.0f }, angle);

					// directionM4 is already transformed by emM4Origin
					p.mat4 *= DirectX::XMMatrixInverse(nullptr, emM4Origin);
				}
				break;

				case ParticleDirectionType::EmitterDirection:
					break;

				case ParticleDirectionType::XAxis:
					directionM4 = DirectX::XMMatrixRotationY(-PI / 2);
					break;

				case ParticleDirectionType::YAxis:
					directionM4 = DirectX::XMMatrixRotationX(PI / 2);
					break;

				case ParticleDirectionType::ZAxis:
					directionM4 = DirectX::XMMatrixRotationZ(-PI / 2);
					break;

				case ParticleDirectionType::YRotationOnly:
					directionM4 = DirectX::XMMatrixRotationY(PI);
					directionM4 *= DirectX::XMMatrixRotationY(Utilities::toRadians(90 - camera.getYaw()));
					break;

				default:
					break;
				}

				Quaternion qX, qY, qZ, qR;
				qX.fromAngleAxis(Utilities::toRadians(rotation.x), Vector3(1, 0, 0));
				qY.fromAngleAxis(Utilities::toRadians(rotation.y), Vector3(0, 1, 0));
				qZ.fromAngleAxis(Utilities::toRadians(rotation.z), Vector3(0, 0, 1));
				qR = emRot * qZ * qY * qX;

				if (dType != ParticleDirectionType::Billboard)
				{
					p.mat4 *= DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{ qR.x, qR.y, qR.z, qR.w });
				}
				else
				{
					p.mat4 *= DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{ qZ.x, qZ.y, qZ.z, qZ.w });
				}

				p.mat4 *= directionM4;
				p.mat4 *= DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);

				p.color = particle->getColor() * p.animation.tryGetColor(p.time);

				if (particle->getType() == ParticleType::Locus)
				{
					// avoid appending to locus history if playback is paused
					if (lastTime != p.time)
					{
						Vector3 temp(p.mat4.r[3].m128_f32[0], p.mat4.r[3].m128_f32[1], p.mat4.r[3].m128_f32[2]);

						LocusHistory history;
						history.pos = temp;
						history.scale = p.scale;
						history.color = p.color;

						// no history
						if (p.locusHistories.size() < 1)
							p.locusHistories.push_back(history);
						else if (p.locusHistories.size() == 1)
						{
							// first entry. append normally
							history.pos = p.locusHistories[0].pos;
							if (p.locusHistories.size() < p.locusHistories.capacity())
								p.locusHistories.push_back(history);

							p.locusHistories[0].pos = temp;
						}
						else
						{
							// shift histories to end of array
							Vector3 last = p.locusHistories[p.locusHistories.size() - 1].pos;
							for (int i = p.locusHistories.size() - 1; i > 0; --i)
								p.locusHistories[i].pos = p.locusHistories[i - 1].pos;

							// then add new history with oldest position
							if (p.locusHistories.size() < p.locusHistories.capacity())
							{
								LocusHistory h = history;
								h.pos = last;
								p.locusHistories.push_back(h);
							}

							p.locusHistories[0].pos = temp;
						}
					}
				}

				if (particle->getUVIndexType() == UVIndexType::Fixed)
				{
					p.UVIndex = particle->getUVIndex();
				}
				else if (maxUV && interval && !((int)p.time % interval) && ((int)p.lastUVChange != (int)p.time))
				{
					switch (type)
					{
					case UVIndexType::ReverseOrder:
					case UVIndexType::InitialRandomReverseOrder:
						--p.UVIndex;
						if (p.UVIndex < 0)
							p.UVIndex = maxUV;
						break;

					case UVIndexType::SequentialOrder:
					case UVIndexType::InitialRandomSequentialOrder:
						p.UVIndex = (p.UVIndex + 1) % maxUV;
						break;

					case UVIndexType::RandomOrder:
						p.UVIndex = Utilities::random(0, maxUV);
						break;
					}

					p.lastUVChange = p.time;
				}

				p.uvScroll.x = p.animation.getValue(AnimationType::UScroll, p.time);
				p.uvScroll.y = p.animation.getValue(AnimationType::VScroll, p.time);

				++aliveCount;
			}
			
			animationSet->markDirty(false);
		}

		void ParticleInstance::create(int n, float startTime, EmissionDirectionType dir, const std::vector<Vector3>& basePos)
		{
			int count = 0;
			for (std::vector<ParticleStatus>::iterator it = pool.begin(); it != pool.end(); ++it)
			{
				if ((*it).dead)
				{
					ParticleStatus& p = (*it);
					auto& particle = reference->getParticle();
					p.startTime = startTime;
					p.time = 0.0f;
					p.dead = false;

					float speed = Utilities::randomize(particle->getSpeed(), particle->getSpeedRandom());
					float deceleration = Utilities::randomize(particle->getDeceleration(), particle->getDecelerationRandom());
					Vector3 direction = Utilities::randomize(particle->getDirection(), particle->getDirectionRandom());
					Vector3 accel = Utilities::randomize(particle->getExternalAccel(), particle->getExternalAccelRandom());
					Vector3 dirAdd;

					switch (dir)
					{
					case EmissionDirectionType::Inward:
						dirAdd = basePos[count];
						dirAdd = -dirAdd;
						break;

					case EmissionDirectionType::Outward:
						dirAdd = basePos[count];
						break;

					default:
						break;
					}

					Vector3 dirResult = direction + dirAdd;
					dirResult.normalise();
					p.direction = dirResult * speed;
					p.basePos = basePos[count];
					p.acceleration = (accel - deceleration) / 3600.0f;
					p.scale = Utilities::randomize(particle->getSize(), particle->getSizeRandom());
					p.UVIndex = particle->getUVIndex();
					p.lastUVChange = p.time;

					Vector3 rotation = Utilities::randomize(particle->getRotation(), particle->getRotationRandom());
					Vector3 rotationAdd = Utilities::randomize(particle->getRotationAdd(), particle->getRotationAddRandom());
					p.rotation = rotation + rotationAdd * (rotationAddCount++ % particle->getMaxCount());

					p.locusHistories.clear();
					if (particle->getType() == ParticleType::Locus)
					{
						int size = Utilities::randomize(particle->getLocusHistorySize(), particle->getLocusHistorySizeRandom());
						p.locusHistories.reserve(size);
					}

					// InitialRandom UVIndex Types
					if ((size_t)particle->getUVIndexType() >= 1 && (size_t)particle->getUVIndexType() < 5)
					{
						Vector2 uvSplit = Vector2(1.0f, 1.0f);
						if (reference->getMaterialNode())
							uvSplit = reference->getMaterialNode()->getMaterial()->getSplit();

						unsigned int maxUV = (uvSplit.x * uvSplit.y) - 1;
						p.UVIndex = Utilities::random(0, maxUV);
					}

					p.animation.buildCache(reference->getAnimationSet());
					++count;
				}

				if (count >= n)
					return;
			}
		}
	}
}

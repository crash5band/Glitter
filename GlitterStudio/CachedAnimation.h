#pragma once
#include "GlitterAnimation.h"
#include "EditorAnimationSet.h"

namespace Glitter
{
	namespace Editor
	{
		class CachedAnimation
		{
		private:
			std::array<std::vector<float>, animationTypeTableSize> cache;
			std::array<bool, animationTypeTableSize> repeatTable;

		public:
			CachedAnimation(std::shared_ptr<EditorAnimationSet> set);
			CachedAnimation();

			void buildCache(std::shared_ptr<EditorAnimationSet> set);
			float getValue(Glitter::AnimationType gType, float time, float fallback = 0.0f);
			Glitter::Vector3 tryGetTranslation(float time);
			Glitter::Vector3 tryGetRotation(float time);
			Glitter::Vector3 tryGetScale(float time);
			Glitter::Color tryGetColor(float time);
		};
	}
}
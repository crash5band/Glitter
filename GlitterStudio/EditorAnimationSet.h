#pragma once
#include "EditorAnimation.h"

constexpr float defaultValue = 0.0f;
constexpr float defaultScale = 1.0f;
constexpr float defaultColor = 255;

namespace Glitter
{
	namespace Editor
	{
		enum class AnimationSetType
		{
			Glitter,
			UV,
			Material,
		};

		class EditorAnimationSet
		{
		private:
			bool dirty;

		public:
			AnimationSetType type;
			std::vector<EditorAnimation> animations;

			EditorAnimationSet(std::vector<GlitterAnimation> list);
			EditorAnimationSet(std::vector<Animation> list);

			void add(EditorAnimation animation, int pos);
			void remove(int pos);

			bool isDirty() const;
			void markDirty(bool v);

			float tryGetValue(Glitter::AnimationType gType, float time, float fallback = 0.0f);
			Glitter::Vector3 tryGetTranslation(float time);
			Glitter::Vector3 tryGetRotation(float time);
			Glitter::Vector3 tryGetScale(float time);
			Glitter::Color tryGetColor(float time);
		};
	}
}

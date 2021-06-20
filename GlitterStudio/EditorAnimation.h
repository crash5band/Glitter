#pragma once
#include "GlitterAnimation.h"
#include "Animation.h"
#include "MathGens.h"
#include "KeyFrame.h"

namespace Glitter
{
	namespace Editor
	{
		struct EditorKeyframe
		{
			float time;
			float value;
			float inParam;
			float outParam;
			float randomRange;
			InterpolationType interpolation;

			EditorKeyframe(Glitter::GlitterKey k)
			{
				time = k.time;
				value = k.value;
				inParam = k.inParam;
				outParam = k.outParam;
				randomRange = k.randomRange;
				interpolation = k.interpolationType;
			}

			EditorKeyframe(Glitter::Keyframe k) : time{ k.frame }, value{ k.value }
			{
				inParam = 0;
				outParam = 0;
				randomRange = 0;
				interpolation = InterpolationType::Linear;
			}

			EditorKeyframe(float frame = 0.0f, float val = 0.0f) :
				time{ frame }, value{ val }, inParam{ 0 }, outParam{ 0 }, randomRange{ 0 }, interpolation{ InterpolationType::Linear }
			{

			}

			inline bool operator==(const EditorKeyframe& k)
			{
				return time == k.time && value == k.value && interpolation == k.interpolation
					&& inParam == k.inParam && outParam == k.outParam && randomRange == k.randomRange;
			}

			inline bool operator!=(const EditorKeyframe& k)
			{
				return !(*this == k);
			}
		};

		class EditorAnimation
		{
		private:
			float start;
			float end;
			float fps;
			unsigned int randomFlags;

		public:
			std::string name;
			Glitter::RepeatType repeat;
			std::vector<EditorKeyframe> keys;
			AnimationType type;

			EditorAnimation(Glitter::GlitterAnimation anim);
			EditorAnimation(Glitter::Animation anim);
			EditorAnimation(float start = 0.0, float end = 0.0, float fps = 60);

			float interpolate(const float time, const EditorKeyframe& k1, const EditorKeyframe& k2);
			float calcRandomRange(float range);
			float verifyKeyOrder(unsigned int keyIndex, float newTime);
			void removeKey(unsigned int pos);
			unsigned int insertKey(const EditorKeyframe& key);

			float getStart() const;
			float getEnd() const;
			float getFPS() const;
			unsigned int getFlags() const;

			void setStart(float t);
			void setEnd(float t);
			void setFPS(float f);
			void setFlags(unsigned int f);

			GlitterAnimation toGlitterAnimation() const;
		};
	}
}
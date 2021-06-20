#include "EditorAnimationSet.h"
#include "Utilities.h"

namespace Glitter
{
	namespace Editor
	{
		EditorAnimationSet::EditorAnimationSet(std::vector<GlitterAnimation> list) : dirty{ true }
		{
			type = AnimationSetType::Glitter;
			for (int i = 0; i < list.size(); ++i)
			{
				animations.push_back(EditorAnimation(list[i]));
			}
		}

		void EditorAnimationSet::add(EditorAnimation animation, int pos)
		{
			if (pos < 0 || pos >= animations.size())
				animations.push_back(animation);
			else
				animations.insert(animations.begin() + pos, animation);
		}

		void EditorAnimationSet::remove(int pos)
		{
			if (pos >= 0 && pos < animations.size())
				animations.erase(animations.begin() + pos);
		}

		bool EditorAnimationSet::isDirty() const
		{
			return dirty;
		}

		void EditorAnimationSet::markDirty(bool v)
		{
			dirty = v;
		}

		float EditorAnimationSet::tryGetValue(AnimationType gType, float time, float fallback)
		{
			return 0.0f;
		}

		Glitter::Vector3 EditorAnimationSet::tryGetTranslation(float time)
		{
			Glitter::Vector3 pos;
			pos.x = tryGetValue(Glitter::AnimationType::Tx, time);
			pos.y = tryGetValue(Glitter::AnimationType::Ty, time);
			pos.z = tryGetValue(Glitter::AnimationType::Tz, time);

			return pos;
		}

		Glitter::Vector3 EditorAnimationSet::tryGetRotation(float time)
		{
			Glitter::Vector3 rot;
			rot.x = tryGetValue(Glitter::AnimationType::Rx, time);
			rot.y = tryGetValue(Glitter::AnimationType::Ry, time);
			rot.z = tryGetValue(Glitter::AnimationType::Rz, time);

			return rot;
		}

		Glitter::Vector3 EditorAnimationSet::tryGetScale(float time)
		{
			Glitter::Vector3 scale;
			scale.x = tryGetValue(Glitter::AnimationType::Sx, time, 1.0f);
			scale.y = tryGetValue(Glitter::AnimationType::Sy, time, 1.0f);
			scale.z = tryGetValue(Glitter::AnimationType::Sz, time, 1.0f);

			scale *= tryGetValue(Glitter::AnimationType::SAll, time, 1.0f);

			return scale;
		}

		Glitter::Color EditorAnimationSet::tryGetColor(float time)
		{
			Glitter::Color col;
			col.r = tryGetValue(Glitter::AnimationType::ColorR, time, 255) / Glitter::COLOR_CHAR;
			col.g = tryGetValue(Glitter::AnimationType::ColorG, time, 255) / Glitter::COLOR_CHAR;
			col.b = tryGetValue(Glitter::AnimationType::ColorB, time, 255) / Glitter::COLOR_CHAR;
			col.a = tryGetValue(Glitter::AnimationType::ColorA, time, 255) / Glitter::COLOR_CHAR;

			return col;
		}
	}
}
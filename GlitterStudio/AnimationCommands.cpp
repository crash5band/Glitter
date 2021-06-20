#include "AnimationCommands.h"

namespace Glitter
{
	namespace Editor
	{
		
		AddAnimationCommand::AddAnimationCommand(std::shared_ptr<EditorAnimationSet>& aSet, EditorAnimation anim, size_t pos) :
			set{ aSet }, animation{ anim }, position{ pos }
		{

		}

		void AddAnimationCommand::execute()
		{
			set.lock()->add(animation, position);
			set.lock()->markDirty(true);
		}

		void AddAnimationCommand::undo()
		{
			set.lock()->remove(position);
			set.lock()->markDirty(true);
		}

		RemoveAnimationCommand::RemoveAnimationCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t pos) :
			set{ aSet }, position{ pos }
		{
			animation = set.lock()->animations[pos];
		}

		void RemoveAnimationCommand::execute()
		{
			set.lock()->remove(position);
			set.lock()->markDirty(true);
		}

		void RemoveAnimationCommand::undo()
		{
			set.lock()->add(animation, position);
			set.lock()->markDirty(true);
		}

		ChangeAnimationCommand::ChangeAnimationCommand(std::shared_ptr<EditorAnimationSet>& aSet, EditorAnimation& anim, size_t pos) :
			set{ aSet }, position{ pos }
		{
			oldValue = set.lock()->animations.at(pos);
			newValue = anim;
		}

		void ChangeAnimationCommand::execute()
		{
			set.lock()->animations[position] = newValue;
			set.lock()->markDirty(true);
		}

		void ChangeAnimationCommand::undo()
		{
			set.lock()->animations[position] = oldValue;
			set.lock()->markDirty(true);
		}

		AddKeyCommand::AddKeyCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t aIndex, int frame) :
			set{ aSet }, animIndex{ aIndex }
		{
			size_t type = (size_t)set.lock()->animations[animIndex].type;
			float value = 0;
			if ((size_t)type >= 6 && (size_t)type < 10)
			{
				// scale
				value = 1;
			}
			else if ((size_t)type >= 10 && (size_t)type < 14)
			{
				// color
				value = 255;
			}

			key = EditorKeyframe{ (float)frame,  value };
		}

		AddKeyCommand::AddKeyCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t aIndex, EditorKeyframe k) :
			set{ aSet }, animIndex{ aIndex }, key{ k }
		{

		}

		void AddKeyCommand::execute()
		{
			keyPos = set.lock()->animations[animIndex].insertKey(key);
			set.lock()->markDirty(true);
		}

		void AddKeyCommand::undo()
		{
			set.lock()->animations[animIndex].removeKey(keyPos);
			set.lock()->markDirty(true);
		}

		RemoveKeyCommand::RemoveKeyCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t aIndex, size_t pos) :
			set{ aSet }, animIndex{ aIndex }, keyPos{ pos }
		{
			key = set.lock()->animations[animIndex].keys[keyPos];
		}

		void RemoveKeyCommand::execute()
		{
			set.lock()->animations[animIndex].removeKey(keyPos);
			set.lock()->markDirty(true);
		}

		void RemoveKeyCommand::undo()
		{
			set.lock()->animations[animIndex].insertKey(key);
			set.lock()->markDirty(true);
		}

		ChangeKeyCommand::ChangeKeyCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t aIndex, EditorKeyframe& k, size_t pos) :
			set{ aSet }, animIndex{ aIndex }, keyPos{ pos }, newValue{ k }
		{
			oldValue = set.lock()->animations[animIndex].keys[keyPos];
		}

		void ChangeKeyCommand::execute()
		{
			set.lock()->animations[animIndex].keys[keyPos] = newValue;
			set.lock()->markDirty(true);
		}

		void ChangeKeyCommand::undo()
		{
			set.lock()->animations[animIndex].keys[keyPos] = oldValue;
			set.lock()->markDirty(true);
		}
	}
}

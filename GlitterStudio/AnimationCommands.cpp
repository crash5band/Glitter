#include "AnimationCommands.h"

AddAnimationCommand::AddAnimationCommand(std::shared_ptr<AnimationNode> &node, Glitter::Animation anim, size_t pos) :
	animationNode{ node }, anim{ anim }, position{ pos }
{

}

void AddAnimationCommand::execute()
{
	animationNode.lock()->add(anim, position);
	animationNode.lock()->buildCache();
}

void AddAnimationCommand::undo()
{
	animationNode.lock()->remove(position);
	animationNode.lock()->buildCache();
}

RemoveAnimationCommand::RemoveAnimationCommand(std::shared_ptr<AnimationNode> &node, size_t pos) :
	animationNode{ node }, position{ pos }
{
	anim = (*animationNode.lock()->getAnimationList())[pos];
}

void RemoveAnimationCommand::execute()
{
	animationNode.lock()->remove(position);
	animationNode.lock()->buildCache();
}

void RemoveAnimationCommand::undo()
{
	animationNode.lock()->add(anim, position);
	animationNode.lock()->buildCache();
}

ChangeAnimationCommand::ChangeAnimationCommand(std::shared_ptr<AnimationNode>& node, size_t pos, Glitter::Animation& val) :
	animationNode{ node }, position{ pos }
{
	oldValue = node->getAnimationList()->at(pos);
	newValue = val;
}

void ChangeAnimationCommand::execute()
{
	animationNode.lock()->getAnimationList()->at(position) = newValue;
}

void ChangeAnimationCommand::undo()
{
	animationNode.lock()->getAnimationList()->at(position) = oldValue;
}

AddKeyCommand::AddKeyCommand(std::shared_ptr<AnimationNode> &node, size_t a, int frame) :
	animationNode{ node }, animIndex{ a }
{
	size_t type = (size_t)node->getAnimationList()->at(a).getType();
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

	key = Glitter::Key{ (float)frame,  value};
}

void AddKeyCommand::execute()
{
	keyPos = animationNode.lock()->getAnimationList()->at(animIndex).insertKey(key);
	animationNode.lock()->buildCache();
}

void AddKeyCommand::undo()
{
	animationNode.lock()->getAnimationList()->at(animIndex).removeKey(keyPos);
	animationNode.lock()->buildCache();
}

RemoveKeyCommand::RemoveKeyCommand(std::shared_ptr<AnimationNode> &node, size_t a, size_t pos) :
	animationNode{ node }, animIndex{ a }, keyPos{ pos }
{
	key = animationNode.lock()->getAnimationList()->at(animIndex).getKeys()[keyPos];
}

void RemoveKeyCommand::execute()
{
	animationNode.lock()->getAnimationList()->at(animIndex).removeKey(keyPos);
	animationNode.lock()->buildCache();
}

void RemoveKeyCommand::undo()
{
	animationNode.lock()->getAnimationList()->at(animIndex).insertKey(key);
	animationNode.lock()->buildCache();
}

ChangeKeyCommand::ChangeKeyCommand(std::shared_ptr<AnimationNode> &node, size_t a, size_t pos, Glitter::Key &k) :
	animationNode{ node }, animIndex{ a }, keyPos{ pos }, newVal{ k }
{
	oldVal = animationNode.lock()->getAnimationList()->at(animIndex).getKeys()[keyPos];
}

void ChangeKeyCommand::execute()
{
	animationNode.lock()->getAnimationList()->at(animIndex).getKeys()[keyPos] = newVal;
	animationNode.lock()->buildCache();
}

void ChangeKeyCommand::undo()
{
	animationNode.lock()->getAnimationList()->at(animIndex).getKeys()[keyPos] = oldVal;
	animationNode.lock()->buildCache();
}
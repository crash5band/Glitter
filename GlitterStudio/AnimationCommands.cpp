#include "AnimationCommands.h"

AddAnimationCommand::AddAnimationCommand(std::shared_ptr<AnimationNode> &node, Glitter::Animation anim, size_t pos) :
	animationNode{ node }, anim{ anim }, position{ pos }
{

}

void AddAnimationCommand::execute()
{
	animationNode->add(anim, position);
	animationNode->buildCache();
}

void AddAnimationCommand::undo()
{
	animationNode->remove(position);
	animationNode->buildCache();
}

RemoveAnimationCommand::RemoveAnimationCommand(std::shared_ptr<AnimationNode> &node, size_t pos) :
	animationNode{ node }, position{ pos }
{
	anim = (*animationNode->getAnimationList())[pos];
}

void RemoveAnimationCommand::execute()
{
	animationNode->remove(position);
	animationNode->buildCache();
}

void RemoveAnimationCommand::undo()
{
	animationNode->add(anim, position);
	animationNode->buildCache();
}

AddKeyCommand::AddKeyCommand(std::shared_ptr<AnimationNode> &node, size_t a, int frame) :
	animationNode{ node }, animIndex{ a }
{
	key = Glitter::Key{ (float)frame };
}

void AddKeyCommand::execute()
{
	keyPos = animationNode->getAnimationList()->at(animIndex).insertKey(key);
	animationNode->buildCache();
}

void AddKeyCommand::undo()
{
	animationNode->getAnimationList()->at(animIndex).removeKey(keyPos);
	animationNode->buildCache();
}

RemoveKeyCommand::RemoveKeyCommand(std::shared_ptr<AnimationNode> &node, size_t a, size_t pos) :
	animationNode{ node }, animIndex{ a }, keyPos{ pos }
{
	key = animationNode->getAnimationList()->at(animIndex).getKeys()[keyPos];
}

void RemoveKeyCommand::execute()
{
	animationNode->getAnimationList()->at(animIndex).removeKey(keyPos);
	animationNode->buildCache();
}

void RemoveKeyCommand::undo()
{
	animationNode->getAnimationList()->at(animIndex).insertKey(key);
	animationNode->buildCache();
}

ChangeKeyCommand::ChangeKeyCommand(std::shared_ptr<AnimationNode> &node, size_t a, size_t pos, Glitter::Key &k) :
	animationNode{ node }, animIndex{ a }, keyPos{ pos }, newVal{ k }
{
	oldVal = animationNode->getAnimationList()->at(animIndex).getKeys()[keyPos];
}

void ChangeKeyCommand::execute()
{
	animationNode->getAnimationList()->at(animIndex).getKeys()[keyPos] = newVal;
	animationNode->buildCache();
}

void ChangeKeyCommand::undo()
{
	animationNode->getAnimationList()->at(animIndex).getKeys()[keyPos] = oldVal;
	animationNode->buildCache();
}
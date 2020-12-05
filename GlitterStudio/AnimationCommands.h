#pragma once
#include "ICommand.h"
#include "AnimationNode.h"
#include <memory>

class AddAnimationCommand : public ICommand
{
private:
	std::weak_ptr<AnimationNode> animationNode;
	Glitter::Animation anim;
	size_t position;

public:
	AddAnimationCommand(std::shared_ptr<AnimationNode> &node, Glitter::Animation anim, size_t pos);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Add Animation"; }
	bool isValid() override { return !animationNode.expired(); }
};

class RemoveAnimationCommand : public ICommand
{
private:
	std::weak_ptr<AnimationNode> animationNode;
	Glitter::Animation anim;
	size_t position;

public:
	RemoveAnimationCommand(std::shared_ptr<AnimationNode> &node, size_t pos);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Remove Animation"; }
	bool isValid() override { return !animationNode.expired(); }
};

class AddKeyCommand : public ICommand
{
private:
	std::weak_ptr<AnimationNode> animationNode;
	size_t animIndex;
	size_t keyPos;
	Glitter::Key key;

public:
	AddKeyCommand(std::shared_ptr<AnimationNode> &node, size_t index, int frame);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Add Key"; }
	bool isValid() override { return !animationNode.expired(); }
};

class RemoveKeyCommand : public ICommand
{
private:
	std::weak_ptr<AnimationNode> animationNode;
	size_t animIndex;
	size_t keyPos;
	Glitter::Key key;

public:
	RemoveKeyCommand(std::shared_ptr<AnimationNode> &node, size_t index, size_t pos);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Remove Key"; }
	bool isValid() override { return !animationNode.expired(); }
};

class ChangeKeyCommand : public ICommand
{
private:
	std::weak_ptr<AnimationNode> animationNode;
	size_t animIndex;
	size_t keyPos;
	Glitter::Key newVal;
	Glitter::Key oldVal;

public:
	ChangeKeyCommand(std::shared_ptr<AnimationNode> &node, size_t index, size_t pos, Glitter::Key& newValue);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Change Key Frame"; }
	bool isValid() override { return !animationNode.expired(); }
};

#include "CommandManager.h"
#include "Logger.h"

std::stack<ICommand*> CommandManager::undoStack;
std::stack<ICommand*> CommandManager::redoStack;

const size_t CommandManager::maxSize = 100;

void CommandManager::pushNew(ICommand* command)
{
	command->execute();
	undoStack.push(command);
	clearRedo();

	Logger::log(Message(MessageType::Normal, command->getDescription()));
}

void CommandManager::undo()
{
	if (undoStack.empty())
		return;

	ICommand* command = undoStack.top();

	undoStack.pop();
	redoStack.push(command);
	command->undo();

	Logger::log(Message(MessageType::Normal, "Undo " + std::string(command->getDescription())));
}

void CommandManager::redo()
{
	if (redoStack.empty())
		return;

	ICommand* command = redoStack.top();

	redoStack.pop();
	undoStack.push(command);
	command->execute();

	Logger::log(Message(MessageType::Normal, "Redo " + std::string(command->getDescription())));
}

void CommandManager::clearUndo()
{
	while (!undoStack.empty())
	{
		ICommand* command = undoStack.top();
		delete command;
		undoStack.pop();
	}
}

void CommandManager::clearRedo()
{
	while (!redoStack.empty())
	{
		ICommand* command = redoStack.top();
		delete command;
		redoStack.pop();
	}
}

void CommandManager::clearAll()
{
	clearUndo();
	clearRedo();

	Logger::log(Message(MessageType::Normal, "Clear Command History"));
}

void CommandManager::clean()
{
	std::stack<ICommand*> filteredUndo, filteredRedo;
	while (undoStack.size())
	{
		ICommand* cmd = undoStack.top();
		if (cmd->isValid())
			filteredUndo.push(cmd);
		else
			delete cmd;

		undoStack.pop();
	}

	while (filteredUndo.size())
	{
		ICommand* cmd = filteredUndo.top();
		if (cmd->isValid())
			undoStack.push(cmd);
		else
			delete cmd;

		filteredUndo.pop();
	}

	while (redoStack.size())
	{
		ICommand* cmd = redoStack.top();
		if (cmd->isValid())
			filteredRedo.push(cmd);
		else
			delete cmd;

		redoStack.pop();
	}

	while (filteredRedo.size())
	{
		ICommand* cmd = filteredRedo.top();
		if (cmd->isValid())
			redoStack.push(cmd);
		else
			delete cmd;

		filteredRedo.pop();
	}
}

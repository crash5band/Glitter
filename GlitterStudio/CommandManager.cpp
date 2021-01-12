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
	while (!command->isValid())
	{
		undoStack.pop();
		delete command;

		if (undoStack.size())
			command = undoStack.top();
		else
			return;
	}

	undoStack.pop();
	command->undo();
	redoStack.push(command);

	Logger::log(Message(MessageType::Normal, "Undo " + std::string(command->getDescription())));
}

void CommandManager::redo()
{
	if (redoStack.empty())
		return;

	ICommand* command = redoStack.top();
	while (!command->isValid())
	{
		redoStack.pop();
		delete command;

		if (redoStack.size())
			command = redoStack.top();
		else
			return;
	}

	redoStack.pop();
	command->execute();
	undoStack.push(command);

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
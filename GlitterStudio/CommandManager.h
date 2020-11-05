#pragma once
#include "ICommand.h"
#include <stack>

class CommandManager
{
private:
	static std::stack<ICommand*> undoStack;
	static std::stack<ICommand*> redoStack;

	static const size_t maxSize;

public:
	static void pushNew(ICommand* command);
	static void undo();
	static void redo();
	static void clearAll();
	static void clearUndo();
	static void clearRedo();

	static std::stack<ICommand*> getUndoHistory() { return undoStack; }
	static std::stack<ICommand*> getRedoHistory() { return redoStack; }
};


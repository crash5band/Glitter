#pragma once

class ICommand
{
public:
	virtual ~ICommand() {};
	virtual void execute() = 0;
	virtual void undo() = 0;
	virtual const char* getDescription() = 0;
	virtual bool isValid() = 0;
};

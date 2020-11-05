#pragma once
#include "ICommand.h"
#include <functional>

template<typename T, typename S>
class ChangePropertyCommand : public ICommand
{
private:
	T* obj;
	S oldValue;
	S newValue;
	std::string description;
	std::_Mem_fn<void (T::*)(S)> setter;

public:
	ChangePropertyCommand(const char* name, T* t, S o, S n, std::_Mem_fn<void (T::*)(S)> func) :
		obj{ t }, oldValue{ o }, newValue{ n }, setter{ func }
	{
		description = "Change ";
		description.append(name);
	}

	void execute() override
	{
		setter(obj, newValue);
	}

	void undo() override
	{
		setter(obj, oldValue);
	}

	const char* getDescription() override
	{
		return description.c_str();
	}
};

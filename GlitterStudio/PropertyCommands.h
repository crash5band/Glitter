#pragma once
#include "ICommand.h"
#include <functional>

template<typename T, typename S>
class ChangePropertyCommand : public ICommand
{
private:
	std::weak_ptr<T> obj;
	S oldValue;
	S newValue;
	std::string description;
	std::_Mem_fn<void (T::*)(S)> setter;

public:
	ChangePropertyCommand(const char* name, std::shared_ptr<T> &t, S o, S n, std::_Mem_fn<void (T::*)(S)> func) :
		obj{ t }, oldValue{ o }, newValue{ n }, setter{ func }
	{
		description = "Change ";
		description.append(name);
	}

	void execute() override
	{
		setter(obj.lock().get(), newValue);
	}

	void undo() override
	{
		setter(obj.lock().get(), oldValue);
	}

	const char* getDescription() override
	{
		return description.c_str();
	}

	bool isValid() override
	{
		return !obj.expired();
	}
};

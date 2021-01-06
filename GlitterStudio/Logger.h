#pragma once
#include <vector>
#include <string>

enum class MessageType
{
	Normal,
	Warning,
	Error
};

struct Message
{
	MessageType type;
	std::string message;

	Message(MessageType t, const std::string& msg) :
		type{ t }, message{ msg }
	{

	}
};

class Logger
{
private:
	static std::vector<Message> mLog;

public:
	static bool hasNew;

	static void show();
	static void clear();
	static void log(Message msg);
};


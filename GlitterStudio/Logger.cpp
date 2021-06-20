#include "Logger.h"
#include "UiHelper.h"
#include "UI.h"
#include <sstream>
#include <ctime>

std::vector<Message> Logger::mLog;
bool Logger::hasNew;

void Logger::clear()
{
	mLog.clear();
}

void Logger::log(Message msg)
{
	std::time_t time = std::time(0);
	std::tm now = *std::localtime(&time);

	std::stringstream msgFull;
	msgFull << now.tm_hour << ":" << now.tm_min << ":" << now.tm_sec << " " << msg.message;

	msg.message = msgFull.str();
	mLog.emplace_back(msg);

	hasNew = true;
}

void Logger::show()
{
	if (ImGui::Begin("Log"))
	{
		if (ImGui::Button("Clear Log", ImVec2(ImGui::GetContentRegionAvail().x, UI::btnHeight)))
			clear();

		ImGui::Separator();
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
		ImGui::BeginChild("Log Messages");

		for (auto& msg : mLog)
		{
			ImVec4 color(1.0f, 1.0f, 1.0f, 1.0f);
			switch (msg.type)
			{
			case MessageType::Warning:
				color = ImVec4(0.95f, 0.8f, 0.04f, 1.0f);
				break;

			case MessageType::Error:
				color = ImVec4(0.95f, 0.23f, 0.05f, 1.0f);
				break;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextWrapped(msg.message.c_str());
			ImGui::PopStyleColor(1);
		}

		if (hasNew)
		{
			ImGui::SetScrollY(ImGui::GetScrollMaxY());
			hasNew = false;
		}

		ImGui::EndChild();
		ImGui::PopStyleColor(1);
	}

	ImGui::End();
}
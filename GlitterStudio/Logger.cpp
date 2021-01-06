#include "Logger.h"
#include "UIFn.h"
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

	std::stringstream str;
	str << now.tm_hour << ":" << now.tm_min << ":" << now.tm_sec << " ";

	std::string msgFull(str.str());
	msgFull.append(msg.message);
	msg.message = msgFull;
	mLog.emplace_back(msg);

	hasNew = true;
}

void Logger::show()
{
	if (ImGui::Begin(logWindow))
	{
		if (ImGui::Button("Clear Log", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f)))
			clear();

		ImGui::Separator();

		if (ImGui::ListBoxHeader("", ImVec2(ImGui::GetContentRegionAvail())))
		{
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
		}

		ImGui::ListBoxFooter();

		if (hasNew)
		{
			float scroll = ImGui::GetScrollMaxY();
			ImGui::SetScrollY(scroll);
			hasNew = false;
		}
	}

	ImGui::End();
}
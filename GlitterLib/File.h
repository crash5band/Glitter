#pragma once
#include <string>

namespace Glitter
{
	class File
	{
	public:
		static std::string getFilePath(const std::string& filepath);
		static std::string getFileName(const std::string& filepath);
		static std::string getFileNameWithoutExtension(const std::string& filepath);
		static std::string getFileExtension(const std::string& filepath);
		static bool exists(const std::string& filepath);
	};
}


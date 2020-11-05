#include "File.h"
#include <iostream>
#include <filesystem>

namespace Glitter
{
	std::string File::getFilePath(const std::string& filepath)
	{
		size_t start = 0;
		size_t end = filepath.find_last_of("\\/");

		return filepath.substr(start, end - start + 1);
	}

	std::string File::getFileName(const std::string& filepath)
	{
		size_t start = filepath.find_last_of("\\/");
		return filepath.substr(start + 1, filepath.size() - (start + 1));
	}

	std::string File::getFileNameWithoutExtension(const std::string& filepath)
	{
		std::string str = getFileName(filepath);
		size_t end = str.find_last_of(".");

		return str.substr(0, end);
	}

	std::string File::getFileExtension(const std::string& filepath)
	{
		size_t start = filepath.find_last_of(".");
		return filepath.substr(start + 1, filepath.size() - (start + 1));
	}

	bool File::exists(const std::string& filepath)
	{
		return (std::filesystem::exists(filepath));
	}
}
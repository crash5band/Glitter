#pragma once
#include <unordered_map>
#include <array>

enum class FileType
{
	Effect,
	Material,
	Model,
	Texture
};

class FileGUI
{
private:
	static const char* getTitle(FileType type);
	static const char* getFilters(FileType type);

public:
	static bool openFileGUI(FileType type, std::string& filename);
	static bool saveFileGUI(FileType type, std::string& filename);
};


#pragma once
#include <unordered_map>
#include <array>

namespace Glitter
{
	namespace Editor
	{
		enum class FileType
		{
			Effect,
			Material,
			Model,
			Texture
		};

		class FileDialog
		{
		private:
			static const char* getTitle(FileType type);
			static const char* getFilters(FileType type);

		public:
			static bool openFolderDialog(std::string& path);
			static bool openFileDialog(FileType type, std::string& filename);
			static bool saveFileDialog(FileType type, std::string& filename);
		};
	}
}

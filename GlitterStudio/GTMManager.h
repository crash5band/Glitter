#pragma once
#include "MaterialNode.h"

namespace Glitter
{
	namespace Editor
	{
		/// <summary>
		/// Manages open materials and provides methods to preview, and close them.
		/// This class is made static so we can change a particle's material from the material list here
		/// </summary>
		class GTMManager
		{
		private:
			static std::vector<std::shared_ptr<MaterialNode>> materialNodes;

		public:
			static void add(std::shared_ptr<MaterialNode>& node);
			static void remove(size_t pos);
			static void clean();
			static void clear();
			static void save(int index, bool saveAs);
			static void saveAll();
			static void duplicate(int index);
			static int count();
			static std::vector<std::shared_ptr<MaterialNode>> getMaterials();

		};
	}
}
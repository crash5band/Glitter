#pragma once
#include "MaterialNode.h"

/// <summary>
/// Manages open materials and provides methods to open, preview, and close them.
/// This class is made static so we can change a particle's material from the material list here
/// </summary>
class MaterialEditor
{
private:
	static std::vector<std::shared_ptr<MaterialNode>> materialNodes;
	static int selection;
	static float texSize;
	static bool showSplits;

public:
	static void add(std::shared_ptr<MaterialNode> &node);
	static void remove(size_t pos);
	static void clean();
	static void clear();
	static void select(int index);
	static std::vector<std::shared_ptr<MaterialNode>> getNodes();
	static int getSelection();
	static int count();

	static void update();
	static void preview();
	static bool materialMenu(int index);
};


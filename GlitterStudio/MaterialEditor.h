#pragma once
#include "MaterialNode.h"

/// <summary>
/// This class is made static so we can change a particle's material from the material list here
/// </summary>

class MaterialEditor
{
private:
	static std::vector<std::shared_ptr<MaterialNode>> materialNodes;
	static int index;
	static float texSize;
	static bool showSplits;

public:
	static void add(std::shared_ptr<MaterialNode> &node);
	static void remove(size_t pos);
	static void clean();
	static void clear();
	static std::vector<std::shared_ptr<MaterialNode>> getNodes();
	static int getSelection();

	static void update();
};


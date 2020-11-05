#pragma once
#include "SubmeshData.h"

class MeshData
{
private:
	std::vector<SubmeshData> submeshes;

public:
	MeshData();
	MeshData(const std::vector<SubmeshData>& list);

	void addSubmesh(SubmeshData &submesh);
	void draw(Shader* shader);
};


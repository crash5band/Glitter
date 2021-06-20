#pragma once
#include "SubmeshData.h"

class MeshData
{
private:
	std::vector<SubmeshData> submeshes;

public:
	MeshData();
	MeshData(const std::vector<SubmeshData>& list);

	void dispose();
	void addSubmesh(SubmeshData &submesh);
	void draw(Shader* shader, float time);
	
	inline std::vector<SubmeshData>& getSubmeshes() { return submeshes; }
};


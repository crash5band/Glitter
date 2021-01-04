#include "MeshData.h"

MeshData::MeshData()
{

}

MeshData::MeshData(const std::vector<SubmeshData>& list)
{
	submeshes = list;
}

void MeshData::dispose()
{
	for (auto& submesh : submeshes)
		submesh.dispose();
}

void MeshData::addSubmesh(SubmeshData &submesh)
{
	submeshes.push_back(submesh);
}

void MeshData::draw(Shader* shader)
{
	for (auto& submesh : submeshes)
		submesh.draw(shader);
}
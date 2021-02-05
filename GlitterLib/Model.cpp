#include "Model.h"
#include "File.h"

namespace Glitter
{
	Model::Model()
	{
		meshes.clear();
		bones.clear();
		name = "";
		filename = "";
		terrainMode = false;
	}

	Model::Model(std::string filename)
	{
		BinaryReader reader(filename, Endianness::BIG);
		this->filename = filename;

		name = filename;
		name = File::getFileNameWithoutExtension(filename);

		terrainMode = File::getFileExtension(filename) == TERRAIN_MODEL_FORAMT;

		if (reader.valid())
		{
			reader.readHeader();
			readRootNode(&reader);
			reader.close();
		}
	}

	Model::Model(BinaryReader* reader, bool terrainMode)
	{
		if (!reader)
			return;

		this->terrainMode = terrainMode;
		if (terrainMode)
		{
			printf("terrain mode not supported. aborting.\n");
			return;
		}

		reader->readHeader();
		readRootNode(reader);
	}

	Model::~Model()
	{
		for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it)
			delete (*it);
		meshes.clear();

		for (std::vector<Bone*>::iterator it = bones.begin(); it != bones.end(); ++it)
			delete (*it);
		bones.clear();
	}

	void Model::getTotalData(std::list<Vertex*> &vertexList, std::list<unsigned int> &facesList, std::list<std::string> &materialNames, std::vector<unsigned int> &materialMappings)
	{
		vertexList = getVertexList();
		facesList = getFacesList();
		materialNames = getMaterialNames();
		materialMappings = getMaterialMappings(materialNames);
	}

	void Model::readRootNode(BinaryReader* reader)
	{
		if (!reader->valid())
			return;

		if (reader->getRootNodeType() != MODEL_ROOT_DYNAMIC_GENERATIONS)
		{
			//printf(std::string(filename + " is not a genrations model. aborting.\n").c_str());
			//return;
		}

		size_t headerAddress = reader->getCurrentAddress();

		unsigned int meshCount = reader->readInt32();
		size_t meshTableAddress = reader->readAddress();

		readSkeleton(reader);

		for (int i = 0; i < meshCount; ++i)
		{
			reader->gotoAddress(meshTableAddress + i*4);
			size_t meshAddress = reader->readAddress();
			reader->gotoAddress(meshAddress);

			Mesh* mesh = new Mesh();
			mesh->read(reader);
			mesh->buildAABB();
			meshes.push_back(mesh);
		}
	}

	void Model::readSkeleton(BinaryReader* reader)
	{
		size_t headerAddress = reader->getCurrentAddress();

		unsigned int unknownTotal = reader->readInt32();
		size_t unknownAddress = reader->readAddress();

		unsigned int totalBones = reader->readInt32();
		size_t boneDefinitionTableAddress = reader->readAddress();
		size_t boneMatrixAddress = reader->readAddress();
		size_t globalAabbAddress = reader->readAddress();

		bones.reserve(totalBones);
		for (int i = 0; i < totalBones; ++i)
		{
			reader->gotoAddress(boneDefinitionTableAddress + (i * 4));
			size_t address = reader->readAddress();
			reader->gotoAddress(address);

			Bone* bone = new Bone();
			bone->readDescription(reader);
			reader->gotoAddress(boneMatrixAddress + (i * 64));
			bone->readMatrix(reader);
			bones.push_back(bone);
		}

		reader->gotoAddress(globalAabbAddress);
		globalAABB = reader->readAABB();
	}

	void Model::writeRootNode(BinaryWriter* writer)
	{
		if (!writer->valid())
			return;

		if (terrainMode)
		{
			printf("terrain mode not supported. aborting.\n");
			return;
		}

		size_t tableAddress = writer->getCurrentAddress();

		unsigned int meshCount = meshes.size();
		size_t modelTableAddress = 0;
		size_t modelNameAddress = 0;

		size_t unknownAddress = 0;
		unsigned int boneCount = bones.size();
		size_t boneDefinitionTableAddress = 0;
		size_t boneMatrixAddress = 0;
		size_t globalAabbAddress = 0;

		writer->writeInt32(meshCount);
		writer->writeNull(12);

		writer->writeInt32(boneCount);
		writer->writeNull(12);

		// write meshes
		modelTableAddress = writer->getCurrentAddress();
		std::vector<unsigned int> meshAddresses;
		writer->writeNull(4 * meshCount);

		for (int i = 0; i < meshCount; ++i)
		{
			meshAddresses.push_back(writer->getCurrentAddress());
			meshes[i]->write(writer);
		}

		writer->gotoAddress(modelTableAddress);
		for (int i = 0; i < meshCount; ++i)
			writer->writeAddress(meshAddresses[i]);

		writer->gotoEnd();
		writer->fixPadding();

		unknownAddress = writer->getCurrentAddress();
		boneDefinitionTableAddress = writer->getCurrentAddress();
		std::vector<unsigned int> boneDefinitionAddresses;

		for (int i = 0; i < boneCount; ++i)
		{
			boneDefinitionAddresses.push_back(writer->getCurrentAddress());
			bones[i]->writeDescription(writer);
		}

		for (int i = 0; i < boneCount; ++i)
		{
			writer->gotoAddress(boneDefinitionTableAddress + i * 4);
			writer->writeAddress(boneDefinitionAddresses[i]);
		}

		writer->gotoEnd();
		boneMatrixAddress = writer->getCurrentAddress();
		for (int i = 0; i < boneCount; ++i)
			writer->write(bones[i]->getMatrix());

		globalAabbAddress = writer->getCurrentAddress();
		writer->write(globalAABB);

		writer->gotoAddress(tableAddress + 4);
		writer->writeAddress(modelTableAddress);
		
		writer->moveAddress(4);
		writer->writeAddress(unknownAddress);
		writer->moveAddress(4);
		writer->writeAddress(boneDefinitionTableAddress);
		writer->writeAddress(boneMatrixAddress);
		writer->writeAddress(globalAabbAddress);
		writer->gotoEnd();
	}

	std::list<Vertex*> Model::getVertexList()
	{
		std::list<Vertex*> allVertices;
		std::list<Vertex*> meshVertices;

		for (int count = 0; count < meshes.size(); ++count)
		{
			meshVertices = meshes[count]->getVertexList();
			for (std::list<Vertex*>::iterator it = meshVertices.begin(); it != meshVertices.end(); ++it)
				allVertices.push_back((*it));
		}

		return allVertices;
	}

	std::list<unsigned int> Model::getFacesList()
	{
		std::list<unsigned int> allFaces;
		std::list<unsigned int> meshFaces;
		unsigned int faceOffset = 0;

		for (int count = 0; count < meshes.size(); ++count)
		{
			meshFaces = meshes[count]->getFacesList();
			for (std::list<unsigned int>::iterator it = meshFaces.begin(); it != meshFaces.end(); ++it)
				allFaces.push_back((*it) + faceOffset);

			faceOffset += meshes[count]->getVertexList().size();
		}

		return allFaces;
	}

	std::list<std::string> Model::getMaterialNames()
	{
		std::list<std::string> allMaterialNames;
		std::list<std::string> meshMaterialNames;

		for (int count = 0; count < meshes.size(); ++count)
		{
			meshMaterialNames = meshes[count]->getMaterialNames();
			for (std::list<std::string>::iterator mIt = meshMaterialNames.begin(); mIt != meshMaterialNames.end(); ++mIt)
			{
				bool found = false;
				for (std::list<std::string>::iterator aIt = allMaterialNames.begin(); aIt != allMaterialNames.end(); ++aIt)
				{
					if ((*mIt) == (*aIt))
					{
						found = true;
						break;
					}
				}

				if (!found)
					allMaterialNames.push_back((*mIt));
			}
		}

		return allMaterialNames;
	}

	std::vector<unsigned int> Model::getMaterialMappings(std::list<std::string>& materialNames)
	{
		std::vector<unsigned int> allMaterialMappings;
		std::vector<unsigned int> meshMaterialMappings;

		for (const auto& mesh : meshes)
		{
			meshMaterialMappings = mesh->getMaterialMappings(materialNames);
			for (const auto& mapping : meshMaterialMappings)
				allMaterialMappings.push_back(mapping);
		}

		return allMaterialMappings;
	}

	void Model::fixVertexFormatForPC()
	{
		for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it)
			(*it)->fixVertexFormatForPC();
	}

	void Model::changeVertexFormat(int type)
	{
		for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it)
			(*it)->changeVertexFormat(type);
	}

	int Model::getBoneIndexByName(std::string& name)
	{
		for (int index = 0; index < bones.size(); ++index)
		{
			if (bones[index]->getName() == name)
				return index;
		}

		return -1;
	}

	unsigned int Model::getEstimatedMemorySize()
	{
		unsigned int totalMemory = 0;
		for (const auto& mesh : meshes)
			totalMemory += mesh->getEstimatedMemorySize();

		return totalMemory;
	}

	std::vector<Mesh*> Model::getMeshes()
	{
		return meshes;
	}

	std::vector<Bone*> Model::getBones()
	{
		return bones;
	}

	AABB Model::getAABB()
	{
		return globalAABB;
	}

	std::string Model::getName()
	{
		return name;
	}

	std::string Model::getFilename()
	{
		return filename;
	}

	void Model::addMesh(Mesh* mesh)
	{
		if (mesh)
			meshes.push_back(mesh);
	}

	void Model::setName(std::string name)
	{
		this->name = name;
	}

	void Model::buildAABB()
	{
		globalAABB.reset();
		for (const auto& mesh : meshes)
			globalAABB.merge(mesh->getAABB());
	}

	void Model::mergeModel(Model* model, Matrix4 transform, float uv2_left, float uv2_right, float uv2_top, float uv2_bottom)
	{
		std::vector<Mesh*> mergeMeshes = model->getMeshes();
		for (std::vector<Mesh*>::iterator it = mergeMeshes.begin(); it != mergeMeshes.end(); it++)
			cloneMesh(*it, transform, uv2_left, uv2_right, uv2_top, uv2_bottom);

		buildAABB();
	}

	void Model::cloneMesh(Mesh* mesh, Matrix4 transform, float uv2_left, float uv2_right, float uv2_top, float uv2_bottom)
	{
		Mesh* cloneMesh = new Mesh(mesh, transform, uv2_left, uv2_right, uv2_top, uv2_bottom);
		meshes.push_back(cloneMesh);
	}

	void Model::save(std::string filename_p, int rootType)
	{
		BinaryWriter writer(filename_p, Endianness::BIG);

		if (writer.valid())
		{
			writer.prepareHeader(rootType);
			writeRootNode(&writer);
			writer.writeHeader(true);
			writer.close();
		}
	}
}
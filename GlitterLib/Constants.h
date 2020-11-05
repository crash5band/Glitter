#pragma once
#include <cstdint>

namespace Glitter
{
	constexpr size_t		fileHeaderDefaultOffset				= 24;
	constexpr size_t		fileHeaderRootTypeAddress			= 4;
	constexpr size_t		fileHeaderRootNodeAddress			= 12;

	constexpr uint32_t		VERTEX_FORMAT_PC					= 1;
	constexpr uint32_t		VERTEX_FORMAT_PC_TERRAIN			= 2;
	constexpr uint32_t		VERTEX_FORMAT_MAX_ENTRIES			= 256;

	constexpr uint32_t		BONE_AFFECT_LIMIT					= 4;

	constexpr uint32_t		MODEL_SUBMESH_SLOT_SOLID			= 0;
	constexpr uint32_t		MODEL_SUBMESH_SLOT_TRANSPARENT		= 1;
	constexpr uint32_t		MODEL_SUBMESH_SLOT_BOOLEAN			= 2;
	constexpr uint32_t		MODEL_SUBMESH_SLOT_WATER			= 3;
	constexpr const char*	MODEL_SUBMESH_UNKNOWN_MATERIAL		= "UnknownMaterial";
	constexpr uint32_t		MODEL_SUBMESH_SLOTS					= 4;
	constexpr uint32_t		MODEL_ROOT_DYNAMIC_GENERATIONS		= 5;
	constexpr const char*	TERRAIN_MODEL_FORAMT				= "terrain-model";

	constexpr uint32_t		MATERIAL_ROOT_GENERATIONS			= 3;

}
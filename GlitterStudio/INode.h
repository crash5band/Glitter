#pragma once
#include "EditorAnimationSet.h"
#include <memory>

namespace Glitter
{
	namespace Editor
	{
		enum class NodeType
		{
			Effect,
			Emitter,
			Particle,
			GTMaterial,
			Model,
			Material,
			Texture
		};

		class INode
		{
		public:
			INode() {};
			virtual ~INode() {};

			virtual void populateInspector() = 0;
			virtual NodeType getNodeType() = 0;
			virtual std::shared_ptr<EditorAnimationSet> getAnimationSet() = 0;
		};
	}
}

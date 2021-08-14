#pragma once
#include "INode.h"
#include "GlitterMaterial.h"
#include "TextureData.h"

namespace Glitter
{
	namespace Editor
	{
		class MaterialNode : public INode
		{
		private:
			std::shared_ptr<GlitterMaterial> material;
			std::shared_ptr<TextureData> texture;

		public:
			MaterialNode(std::shared_ptr<GlitterMaterial>& mat);
			MaterialNode(std::shared_ptr<MaterialNode>& rhs);

			std::shared_ptr<GlitterMaterial> getMaterial();
			std::shared_ptr<TextureData> getTexture();
			void changeTexture(std::shared_ptr<TextureData> tex);

			virtual NodeType getNodeType() override;
			virtual void populateInspector() override;
			virtual std::shared_ptr<EditorAnimationSet> getAnimationSet() override;
		};
	}
}
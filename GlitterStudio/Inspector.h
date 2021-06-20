#pragma once
#include "INode.h"
#include "AnimationTimeline.h"

class TextureData;

namespace Glitter
{
	namespace Editor
	{
		enum class ColorPickerMode
		{
			Normal,
			HueWheel
		};

		class Inspector
		{
		private:
			std::weak_ptr<INode> node;
			ColorPickerMode colorMode;
			bool showTextureSplits;
			int texSize;

		public:
			Inspector();

			void update();
			void setNode(std::shared_ptr<INode> node);
			void previewTexture(std::shared_ptr<TextureData> texture, Glitter::Vector2 split, bool showSplits, bool limitSizeByWindow);
		};
	}
}
#pragma once
#include "ModelViewer.h"
#include "Inspector.h"

namespace Glitter
{
	namespace Editor
	{
		class ModelEditor
		{
		private:
			int modelIndex;
			int textureIndex;

			Inspector inspector;
			AnimationTimeline animationTimeline;
			ModelViewer modelViewer;

			void updateListview();

		public:
			ModelEditor();

			void openModel();
			void openModelFolder();
			void openTexture();
			void openTextureFolder(const std::string& directory);
			void closeModel(int index);
			void closeAllModels();
			void closeTexture(int index);
			void closeAllTextures();
			void updateMenubar();
			void update(Renderer* renderer, float deltaT);
			void cleanUp();
		};
	}
}

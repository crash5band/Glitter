#pragma once
#include "IViewer.h"
#include "Viewport.h"
#include "ModelData.h"

namespace Glitter
{
	namespace Editor
	{
		class ModelViewer : public IViewer
		{
		private:
			Viewport viewport;
			std::shared_ptr<ModelData> selectedModel;
			int index;
			float time;
			float maxTime;
			float playbackSpeed;
			bool playing;
			bool loop;
			bool drawGrid;

			virtual void updatePreview(Renderer* renderer, float deltaT);
			void modelProperties();

		public:
			ModelViewer();

			void setModel(std::shared_ptr<ModelData>& model);
			virtual void update(Renderer* renderer, float deltaT);
			virtual void togglePlayback();
			virtual void stopPlayback();
			virtual void stepFrame();
			virtual void replay();
			virtual bool isPlaying();
			virtual bool isLoop();
			void openFolder();
			void open();
		};
	}
}

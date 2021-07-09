#pragma once
#include "IViewer.h"
#include "Viewport.h"

namespace Glitter
{
	namespace Editor
	{
		class GlitterPlayer : public IViewer
		{
		private:
			float time;
			float maxTime;
			float playbackSpeed;
			bool playing;
			bool loop;
			bool drawGrid;
			bool drawRefModel;
			bool playOnSelect;
			EffectNode* selectedEffect;
			Viewport viewport;
			std::shared_ptr<ModelData> refModel;

			virtual void updatePreview(Renderer* renderer, float deltaT);

		public:
			GlitterPlayer();
			~GlitterPlayer();

			virtual void update(Renderer* renderer, float deltaT);
			virtual void togglePlayback();
			virtual void stopPlayback();
			virtual void replay();
			virtual void stepFrame();
			virtual bool isPlaying();
			virtual bool isLoop();
			void setEffect(EffectNode* node);
			void changeRefMesh(const std::string& path);
			bool isEffectLoop();
		};
	}
}

#pragma once
#include "Viewport.h"
#include "EffectNode.h"

class Renderer;

namespace Glitter
{
	namespace Editor
	{
		class GlitterPlayer
		{
		private:
			float time;
			float maxTime;
			float playbackSpeed;
			bool playing;
			bool loop;
			bool drawGrid;
			bool playOnSelect;
			EffectNode* selectedEffect;
			Renderer* renderer;
			Viewport viewport;

			void updatePreview(Renderer* renderer, float deltaT);

		public:
			GlitterPlayer();
			~GlitterPlayer();

			void update(Renderer* renderer, float deltaT);
			void togglePlayback();
			void stopPlayback();
			void replay();
			bool isPlaying();
			bool isLoop();
			void setEffect(EffectNode* node);
			bool isEffectLoop();
		};
	}
}

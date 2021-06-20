#pragma once
#include "Renderer.h"

namespace Glitter
{
	namespace Editor
	{
		class IViewer
		{
		private:
			virtual void updatePreview(Renderer* renderer, float deltaT) = 0;

		public:
			IViewer() {}
			~IViewer() {}

			virtual void update(Renderer* renderer, float deltaT) = 0;
			virtual void togglePlayback() = 0;
			virtual void stopPlayback() = 0;
			virtual void replay() = 0;
			virtual bool isPlaying() = 0;
			virtual bool isLoop() = 0;
		};
	}
}

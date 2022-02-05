#pragma once
#include "Camera.h"
#include "Light.h"
#include "RenderTarget.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

namespace Glitter
{
	namespace Editor
	{
		class Viewport
		{
		private:
			Camera camera;
			Light light;
			Engine::RenderTarget fBuffer;
			ImRect viewArea;
			Glitter::Vector2 size;
			bool lightEnabled;
			bool pendingScreenshot;
			int drawMode;
			float renderScale = 1.50f;

			void saveScreenshot();

		public:
			Viewport();
			~Viewport();

			void use();
			void end();
			void cameraControl();
			void resetCamera();
			void lightControl();
			void toggleLight();
			void renderingControl();
			void screenshotControl();

			Glitter::Vector2 getSize() const;
			Camera getCamera() const;
			Light getLight() const;
			bool isLightEnabled() const;
		};
	}
}

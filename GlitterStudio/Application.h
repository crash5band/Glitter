#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "ParticleEditor.h"
#include "ModelEditor.h"
#include "InputListener.h"

class Renderer;

namespace Glitter
{
	namespace Editor
	{
		class Application
		{
		private:
			static std::string appDir;
			static std::string shadersDir;
			static std::string fontsDir;
			static std::string screenshotsDir;
			static std::string version;

			std::string imguiConfig = "imgui.ini";
			std::string editorConfig = "config.dat";

			bool imguiDemoWindow;
			bool fpsMeter;
			bool vsync;
			bool aboutOpen;
			bool debugView;

			GLFWwindow* window;
			ImGuiID pDockSpaceID;

			ParticleEditor *particleEditor;
			Renderer *renderer;
			InputListener inputListener;

			float lastFrame;
			float frameDelta;

			std::string getVersion();

			bool initOpenGL();
			bool initImgui();
			void setImguiStyle();
			void debugInfo();
			void updateMenubar();

		public:
			Application(const std::string& dir);
			~Application();

			void go();
			void frameTime();
			void about();
			void processInput();
			void updateUI();
			void setDirectory(const std::string& dir);
			void loadSettings(const std::string& filename);
			void saveSettings(const std::string& filename);

			static std::string getAppVersion();
			static std::string getDirectory();
			static std::string getScreenshotsDirectory();
		};
	}
}
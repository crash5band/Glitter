#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "ParticleEditor.h"
#include "ModelEditor.h"
#include "InputLIstener.h"

class Renderer;

namespace Glitter
{
	namespace Editor
	{
		enum class EditorMode
		{
			Particle,
			Model
		};

		class Application
		{
		private:
			EditorMode mode;
			std::string appDir;
			std::string shadersDir;
			std::string fontsDir;
			std::string imguiConfig = "imgui.ini";
			std::string editorConfig = "config.dat";
			bool imguiDemoWindow;
			bool fpsMeter;
			bool vsync;
			bool aboutOpen;
			bool debugView;

			GLFWwindow* window;
			ImGuiID pDockSpaceID;
			ImGuiID mDockSpaceID;

			ParticleEditor *particleEditor;
			ModelEditor* modelEditor;
			Renderer *renderer;
			InputListener inputListener;

			float lastFrame;
			float frameDelta;

			bool initOpenGL();
			bool initImgui();
			void setImguiStyle();
			void debugInfo();

			void updateMenubar();

		public:
			static int screenWidth;
			static int screenHeight;

			Application(const std::string& dir);
			~Application();

			void go();
			void frameTime();
			void about();
			void processInput();
			void updateUI();
			void getVersion(char* buffer);
			void setDirectory(const std::string& dir);
			void loadSettings(const std::string& filename);
			void saveSettings(const std::string& filename);
		};
	}
}
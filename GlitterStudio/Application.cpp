#include "Application.h"
#include "ResourceManager.h"
#include "CommandManager.h"
#include "Utilities.h"
#include "File.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

namespace Glitter
{
	namespace Editor
	{
		Application::Application(const std::string& dir) : mode{ EditorMode::Particle }, vsync{ true }
		{
			screenWidth = 1366;
			screenHeight = 768;
			imguiDemoWindow = false;
			fpsMeter = false;
			aboutOpen = false;
			debugView = false;

			setDirectory(dir);
			initOpenGL();
			initImgui();
			setImguiStyle();

			ResourceManager::loadShader("BillboardParticle", shadersDir + "BillboardParticle");
			ResourceManager::loadShader("MeshParticle", shadersDir + "MeshParticle");
			ResourceManager::loadShader("Grid", shadersDir + "GridShader");
			ResourceManager::loadShader("Mesh", shadersDir + "Mesh");
			
			particleEditor = new ParticleEditor();
			modelEditor = new ModelEditor();
			renderer = new Renderer();

			pDockSpaceID = 3939;
			mDockSpaceID = 4239;

			Utilities::initRandom();
		}

		Application::~Application()
		{

		}

		void Application::setDirectory(const std::string& dir)
		{
			appDir = Glitter::File::getFilePath(dir);
			shadersDir = appDir + "Res\\Shaders\\";
			fontsDir = appDir + "Res\\Fonts\\";
		}

		void Application::frameTime()
		{
			float currentFrame = glfwGetTime();
			frameDelta = currentFrame - lastFrame;
			lastFrame = currentFrame;
		}

		void Application::processInput()
		{
			inputListener.update(window);
			if (ImGui::GetIO().WantCaptureKeyboard)
				return;

			if (inputListener.isDown(GLFW_KEY_LEFT_CONTROL) || inputListener.isDown(GLFW_KEY_RIGHT_CONTROL))
			{
				if (inputListener.isTapped(GLFW_KEY_Z))
					CommandManager::undo();
				else if (inputListener.isTapped(GLFW_KEY_Y))
					CommandManager::redo();
				else if (inputListener.isTapped(GLFW_KEY_C))
					particleEditor->copy();
				else if (inputListener.isTapped(GLFW_KEY_V))
					particleEditor->paste();
				else if (inputListener.isTapped(GLFW_KEY_O))
				{
					std::string name;
					if (mode == EditorMode::Particle)
					{
						if (FileGUI::openFileGUI(FileType::Effect, name))
							particleEditor->open(name);
					}
					else if (mode == EditorMode::Model)
					{
						modelEditor->openModel();
					}
				}
				else if (inputListener.isTapped(GLFW_KEY_S))
				{
					if (mode == EditorMode::Particle)
					{
						bool saveAs = inputListener.isDown(GLFW_KEY_LEFT_SHIFT) || inputListener.isDown(GLFW_KEY_RIGHT_SHIFT);
						particleEditor->save(particleEditor->getSelectedEffect(), saveAs);
					}
				}
			}
			else
			{
				if (inputListener.isDown(GLFW_KEY_LEFT_ALT) || inputListener.isDown(GLFW_KEY_RIGHT_ALT))
				{
					if (inputListener.isTapped(GLFW_KEY_O))
					{
						std::string name;
						if (mode == EditorMode::Particle)
						{
							if (FileGUI::openFileGUI(FileType::Material, name))
								particleEditor->open(name);
						}
						else if (mode == EditorMode::Model)
						{
							modelEditor->openTexture();
						}
					}

					if (inputListener.isTapped(GLFW_KEY_S))
					{
						bool saveAs = inputListener.isDown(GLFW_KEY_LEFT_SHIFT) || inputListener.isDown(GLFW_KEY_RIGHT_SHIFT);
						particleEditor->saveMaterial(particleEditor->getSelectedMaterial(), saveAs);
					}
				}
			}
		}

		void Application::go()
		{
			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents();
				frameTime();

				glDisable(GL_FRAMEBUFFER_SRGB);

				glClearColor(0.1, 0.1, 0.1, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				processInput();
				updateUI();

				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					GLFWwindow* backup = glfwGetCurrentContext();
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();
					glfwMakeContextCurrent(backup);
				}

				glEnable(GL_FRAMEBUFFER_SRGB);

				glfwSwapBuffers(window);
			}

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();

			glfwDestroyWindow(window);
			glfwTerminate();
		}
	}
}
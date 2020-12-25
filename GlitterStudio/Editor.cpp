#include "Editor.h"
#include "File.h"
#include "FileGUI.h"
#include "ResourceManager.h"
#include "MaterialEditor.h"
#include "Stopwatch.h"
#include "Utilities.h"
#include "Logger.h"
#include <Windows.h>

int Editor::screenWidth = 1366;
int Editor::screenHeight = 768;
bool Editor::resizing = false;
DebugWindows Editor::debugWindows;
EditorSetting Editor::editorSettings;
std::vector<DeltaTime> Editor::times;

Editor::Editor() : frameDelta{ 0 }, lastFrame{ 0 }
{
	char buf[_MAX_PATH];
	GetModuleFileNameA(NULL, buf, _MAX_PATH);
	appDir = Glitter::File::getFilePath(buf);
	imguiINIDir = appDir + "imgui.ini";

	initOpenGl();
	initImgui();
	setImguiStyle();
	
	reset();

	ResourceManager::loadShader("BillboardParticle", appDir + "/Res/Shaders/BillboardParticle.vert", appDir + "/Res/Shaders/BillboardParticle.frag");
	ResourceManager::loadShader("MeshParticle", appDir + "/Res/Shaders/MeshParticle.vert", appDir + "/Res/Shaders/MeshParticle.frag");

	inspector = new Inspector();
	player = new GlitterPlayer();
	inputManager = new InputManager();
	Editor::editorSettings.vsync = true;

	dockspaceID = 39765;

	Utilities::initRandom();
}

void Editor::setScreenDimensions(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
}

bool Editor::openGlitterFile(const std::string& filename)
{
	if (!Glitter::File::exists(filename))
	{
		Logger::log(Message( MessageType::Error, std::string("Failed to load %s. File does not exist", filename.c_str() )));
		return false;
	}

	std::string extension = Glitter::File::getFileExtension(filename);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	if (extension == "gte")
	{
		auto effect = std::make_shared<Glitter::GlitterEffect>(filename);
		auto effectNode = std::make_shared<EffectNode>(effect);
		effectNodes.emplace_back(effectNode);

		for (auto& particle : effect->getParticles())
		{
			std::string filepath = Glitter::File::getFilePath(filename);
			openGlitterFile(filepath + particle->getMaterial() + ".gtm");
		}

		std::vector<std::shared_ptr<MaterialNode>> materials = MaterialEditor::getNodes();
		for (auto& p : effectNode->getParticleNodes())
		{
			for (int i = 0; i < MaterialEditor::getNodes().size(); ++i)
			{
				if (Glitter::File::getFilePath(effect->getFilename()) == Glitter::File::getFilePath(materials[i]->getMaterial()->getFilename())
					&& materials[i]->getMaterial()->getName() == p->getParticle()->getMaterial())
					p->setMaterial(materials[i]);
			}
		}
	}
	else if (extension == "gtm")
	{
		bool exists = false;
		std::vector<std::shared_ptr<MaterialNode>> materials = MaterialEditor::getNodes();
		for (std::vector<std::shared_ptr<MaterialNode>>::iterator it = materials.begin(); it != materials.end(); ++it)
		{
			if ((*it)->getMaterial()->getFilename() == filename)
			{
				exists = true;
				break;
			}
		}
		
		if (!exists)
		{
			auto material = std::make_shared<Glitter::GlitterMaterial>(filename);
			auto materialNode = std::make_shared<MaterialNode>(material);
			MaterialEditor::add(materialNode);
		}
	}
	else
	{
		Logger::log(Message( MessageType::Error, filename + std::string(" is not a supported file type" )));
		return false;
	}

	return true;
}

void Editor::closeEffect(size_t index)
{
	std::string effName = effectNodes.at(index)->getEffect()->getName();
	effectNodes.erase(effectNodes.begin() + index);

	MaterialEditor::clean();
	ResourceManager::cleanModels();
	ResourceManager::cleanTextures();
	Logger::log(Message(MessageType::Normal, "Closed effect " + effName + "."));
}

void Editor::saveEffect(bool saveAs)
{
	if (selectedParent > -1)
	{
		std::string name = effectNodes[selectedParent]->getEffect()->getFilename();
		if (name.size() && !saveAs)
		{
			effectNodes[selectedParent]->getEffect()->write(name);
		}
		else
		{
			name = effectNodes[selectedParent]->getEffect()->getName();
			if (FileGUI::saveFileGUI(FileType::Effect, name))
			{
				if (Glitter::File::getFileExtension(name) != "gte")
					name += ".gte";

				effectNodes[selectedParent]->getEffect()->write(name);
			}
		}

		Logger::log(Message( MessageType::Normal, std::string("Saved effect " + name )));
	}
}

void Editor::saveMaterial(bool saveAs)
{
	std::vector<std::shared_ptr<MaterialNode>> nodes = MaterialEditor::getNodes();
	int index = MaterialEditor::getSelection();
	if (index > -1)
	{
		std::string name = nodes[index]->getMaterial()->getFilename();
		if (name.size() && !saveAs)
		{
			nodes[index]->getMaterial()->write(name);
		}
		else
		{
			name = nodes[index]->getMaterial()->getName();
			if (FileGUI::saveFileGUI(FileType::Material, name))
			{
				if (Glitter::File::getFileExtension(name) != "gtm")
					name += ".gtm";

				nodes[index]->getMaterial()->write(name);
			}
		}

		Logger::log(Message(MessageType::Normal, std::string("Saved material " + name )));
	}
}

void Editor::processInput()
{
	if (io->WantCaptureKeyboard)
		return;

	if (inputManager->isDown(GLFW_KEY_LEFT_CONTROL) || inputManager->isDown(GLFW_KEY_RIGHT_CONTROL))
	{
		if (inputManager->isDown(GLFW_KEY_LEFT_ALT) || inputManager->isDown(GLFW_KEY_RIGHT_ALT))
		{
			if (inputManager->isTapped(GLFW_KEY_S))
				saveEffect(true);
		}
		else
		{
			if (inputManager->isTapped(GLFW_KEY_Z))
				CommandManager::undo();
			else if (inputManager->isTapped(GLFW_KEY_Y))
				CommandManager::redo();
			else if (inputManager->isTapped(GLFW_KEY_O))
			{
				std::string name;
				if (FileGUI::openFileGUI(FileType::Effect, name))
					openGlitterFile(name);
			}
			else if (inputManager->isTapped(GLFW_KEY_S))
				saveEffect(false);

			if (inputManager->isTapped(GLFW_KEY_SPACE))
				player->stopPlayback();
		}
	}
	else
	{
		if (inputManager->isTapped(GLFW_KEY_SPACE))
			player->togglePlayback();
	}
}

void Editor::reset()
{
	selectedParent = -1;
	selectedChild = -1;
	effectNodes.clear();
	MaterialEditor::clear();
	CommandManager::clearAll();
	ResourceManager::disposeAll();
	Logger::clear();

}

void Editor::go()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		frameTime();
		times.clear();
		{
			Stopwatch mainTimer("Total");

			glClearColor(0.1, 0.1, 0.1, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			inputManager->update(window);
			processInput();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			resizeLayout(dockspaceID, screenWidth, screenHeight);
			initLayout(dockspaceID);

			//ImGui::ShowDemoWindow();

			updateGlitterTreeView();
			MaterialEditor::update();
			inspector->update();
			player->update(frameDelta);
			Logger::show();
		}

		debugInfo();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

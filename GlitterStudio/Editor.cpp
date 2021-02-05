#include "Editor.h"
#include "File.h"
#include "FileGUI.h"
#include "ResourceManager.h"
#include "MaterialEditor.h"
#include "CommandManager.h"
#include "Utilities.h"
#include "Logger.h"
#include <Windows.h>
#include <filesystem>

int Editor::screenWidth = 1366;
int Editor::screenHeight = 768;
bool Editor::resizing = false;
DebugWindows Editor::debugWindows;
EditorSetting Editor::editorSettings;
std::vector<DeltaTime> Editor::times;

Editor::Editor() : frameDelta{ 0 }, lastFrame{ 0 }, selectedParent{ -1 }, selectedChild{ -1 }
{
	char buf[_MAX_PATH];
	GetModuleFileNameA(NULL, buf, _MAX_PATH);
	appDir = Glitter::File::getFilePath(buf);

	initOpenGl();
	initImgui();
	setImguiStyle();

	ResourceManager::loadShader("BillboardParticle", shadersDir + "BillboardParticle.vert", shadersDir + "BillboardParticle.frag");
	ResourceManager::loadShader("MeshParticle", shadersDir + "MeshParticle.vert", shadersDir + "MeshParticle.frag");
	ResourceManager::loadShader("Grid", shadersDir + "GridShader.vert", shadersDir + "GridShader.frag");

	inspector = new Inspector();
	player = new GlitterPlayer();
	inputManager = new InputManager();
	Editor::editorSettings.vsync = true;
	Editor::editorSettings.fpsCounter = false;
	Editor::editorSettings.matPreview = true;

	dockspaceID = 39765;

	Utilities::initRandom();
}

void Editor::setScreenDimensions(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
}

void Editor::logTime(const char* name, const Stopwatch& t)
{
	Editor::times.emplace_back(DeltaTime(name, t.elapsed()));
}

bool Editor::openGlitterFile(const std::string& filename)
{
	if (!Glitter::File::exists(filename))
	{
		Logger::log(Message(MessageType::Error, "Failed to load " + filename + ". File not found."));
		return false;
	}

	std::string extension = Glitter::File::getFileExtension(filename);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	if (extension == "gte")
	{
		auto effect = std::make_shared<Glitter::GlitterEffect>(filename);
		auto effectNode = std::make_shared<EffectNode>(effect);
		effectNodes.emplace_back(effectNode);

		std::string filepath = Glitter::File::getFilePath(filename);

		for (auto& particle : effect->getParticles())
			openGlitterFile(filepath + particle->getMaterial() + ".gtm");

		std::vector<std::shared_ptr<MaterialNode>> materials = MaterialEditor::getNodes();
		for (auto& p : effectNode->getParticleNodes())
		{
			for (auto& m : materials)
			{
				if (filepath == Glitter::File::getFilePath(m->getMaterial()->getFilename())
					&& m->getMaterial()->getName() == p->getParticle()->getMaterial())
					p->setMaterial(m);
			}
		}
	}
	else if (extension == "gtm")
	{
		std::vector<std::shared_ptr<MaterialNode>> materials = MaterialEditor::getNodes();
		for (const auto& m : materials)
		{
			if (m->getMaterial()->getFilename() == filename)
				return false;
		}

		auto material = std::make_shared<Glitter::GlitterMaterial>(filename);
		auto materialNode = std::make_shared<MaterialNode>(material);
		MaterialEditor::add(materialNode);
	}
	else
	{
		Logger::log(Message(MessageType::Error, filename + " is not a supported file type."));
		return false;
	}

	return true;
}

void Editor::openFolder(const std::string& directory)
{
	if (!std::filesystem::exists(directory))
		return;

	Logger::log(Message(MessageType::Normal, "Looking for GTE files in " + directory + "..."));

	std::vector<std::string> files;
	for (const auto& file : std::filesystem::directory_iterator(directory))
	{
		std::string extension = file.path().extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if (extension == ".gte")
			files.emplace_back(file.path().string());
	}

	Logger::log(Message(MessageType::Normal, "Found " + std::to_string(files.size()) + " GTE files."));

	for (const std::string& file : files)
		openGlitterFile(file);
}

void Editor::closeEffect(int index)
{
	if (index < 0 || index >= effectNodes.size())
		return;

	std::string effName = effectNodes.at(index)->getEffect()->getName();
	effectNodes.erase(effectNodes.begin() + index);
	selectedParent = selectedChild = -1;

	inspector->setNode(nullptr);
	player->setEffect(nullptr);

	cleanUp();
	Logger::log(Message(MessageType::Normal, "Closed effect " + effName + "."));
}

void Editor::closeAllEffects()
{
	effectNodes.clear();
	selectedParent = selectedChild = -1;

	inspector->setNode(nullptr);
	player->setEffect(nullptr);
	cleanUp();

	Logger::log(Message(MessageType::Normal, "Closed all effects."));
}

void Editor::cleanUp()
{
	CommandManager::clean();
	MaterialEditor::clean();
	ResourceManager::cleanModels();
	ResourceManager::cleanTextures();
}

void Editor::saveEffect(int index, bool saveAs)
{
	if (index > -1 && index < effectNodes.size())
	{
		std::string name = effectNodes[index]->getEffect()->getFilename();
		bool ok = false;
		if (name.size() && !saveAs)
		{
			ok = true;
		}
		else
		{
			name = effectNodes[index]->getEffect()->getName();
			if (FileGUI::saveFileGUI(FileType::Effect, name))
			{
				if (Glitter::File::getFileExtension(name) != "gte")
					name += ".gte";

				ok = true;
			}
		}

		if (ok)
		{
			effectNodes[index]->getEffect()->write(name);
			Logger::log(Message(MessageType::Normal, "Saved effect " + name));

			// save materials used by effect's particles
			auto &particles = effectNodes[index]->getParticleNodes();
			auto materials = MaterialEditor::getNodes();
			for (auto& particle : particles)
			{
				for (int m = 0; m < materials.size(); ++m)
				{
					if (particle->getMaterialNode())
					{
						if (particle->getMaterialNode()->getMaterial()->getName() == materials[m]->getMaterial()->getName())
							saveMaterial(m, false);
					}
				}
			}
		}
	}
}

void Editor::saveMaterial(int index, bool saveAs)
{
	std::vector<std::shared_ptr<MaterialNode>> nodes = MaterialEditor::getNodes();
	if (index > -1 && index < nodes.size())
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

		Logger::log(Message(MessageType::Normal, "Saved material " + name));
	}
}

void Editor::getVersion(char* buffer)
{
	char filename[1024];
	strcpy_s(filename, std::string(appDir + "GlitterStudio.exe").c_str());

	DWORD  verHandle = 0;
	UINT   size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD  verSize = GetFileVersionInfoSize(filename, &verHandle);

	int major = 0, minor = 0, build = 0, rev = 0;
	if (verSize != NULL)
	{
		LPSTR verData = new char[verSize];

		if (GetFileVersionInfo(filename, verHandle, verSize, verData))
		{
			if (VerQueryValue(verData, "\\", (VOID FAR * FAR*) & lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{
						major = (verInfo->dwFileVersionMS >> 16) & 0xffff;
						minor = (verInfo->dwFileVersionMS >> 0) & 0xffff;
						rev = (verInfo->dwFileVersionLS >> 16) & 0xffff;
					}
				}
			}
		}
		delete[] verData;
	}

	sprintf(buffer, "%d.%d.%d.%d", major, minor, build, rev);
}

void Editor::about()
{
	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Appearing);

	if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_NoResize))
	{
		ImGui::Text("Glitter Studio\nCopyright (C) 2021 Crash5b\n\n");
		ImGui::Separator();

		ImGui::Columns(2, "about_cols", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x / 2.0f);

		const int fieldCount = 2;
		static const char* fields[fieldCount]{ "Version", "Github" };
	
		for (int f = 0; f < fieldCount; ++f)
			ImGui::Text(fields[f]);

		ImGui::NextColumn();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

		char buffer[64];
		getVersion(buffer);

		ImGui::Text(buffer);
		ImGui::Text("https://github.com/crash5band/Glitter");

		ImGui::Columns(1);
		ImGui::Separator();

		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - 35.0f));
		if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 25.0f)))
			ImGui::CloseCurrentPopup();
			
		ImGui::EndPopup();
	}
}

void Editor::processInput()
{
	//if (io->WantCaptureKeyboard)
		//return;

	if (inputManager->isDown(GLFW_KEY_LEFT_CONTROL) || inputManager->isDown(GLFW_KEY_RIGHT_CONTROL))
	{
		if (inputManager->isDown(GLFW_KEY_LEFT_ALT) || inputManager->isDown(GLFW_KEY_RIGHT_ALT))
		{
			if (inputManager->isTapped(GLFW_KEY_S))
				saveEffect(selectedParent, true);
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
		}

		if (inputManager->isDown(GLFW_KEY_LEFT_SHIFT) || inputManager->isDown(GLFW_KEY_RIGHT_SHIFT))
		{
			if (inputManager->isTapped(GLFW_KEY_S))
				saveMaterial(MaterialEditor::getSelection(), false);
		}
		else
		{
			if (inputManager->isTapped(GLFW_KEY_S))
				saveEffect(selectedParent, false);
		}
	}
	else
	{
		if (inputManager->isTapped(GLFW_KEY_DELETE))
		{
			if (selectedParent == -1)
				return;
			else
			{
				size_t emitterCount = effectNodes[selectedParent]->getEmitterNodes().size();
				size_t particleCount = effectNodes[selectedParent]->getParticleNodes().size();

				if (selectedChild >= 0 && selectedChild < emitterCount)
					removeEmitter(effectNodes[selectedParent], selectedChild);
				else if (selectedChild >= emitterCount && selectedChild < emitterCount + particleCount)
					removeParticle(effectNodes[selectedParent], selectedChild - emitterCount);

				selectedChild = -1;
			}
		}

		if (inputManager->isDown(GLFW_KEY_LEFT_ALT) || inputManager->isDown(GLFW_KEY_RIGHT_ALT))
		{
			if (inputManager->isTapped(GLFW_KEY_O))
			{
				std::string name;
				if (FileGUI::openFileGUI(FileType::Material, name))
					openGlitterFile(name);
			}
		}
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
		mainTimer.reset();

		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		inputManager->update(window);
		processInput();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		resizeLayout(dockspaceID, screenWidth, screenHeight);
		initLayout(dockspaceID, screenWidth, screenHeight);

		if (debugWindows.imguiDemoOpen)
			ImGui::ShowDemoWindow(&debugWindows.imguiDemoOpen);

		updateGlitterTreeView();
		MaterialEditor::update();
		inspector->update();
		player->update(frameDelta);
		Logger::show();

		logTime("Main Timer", mainTimer);

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

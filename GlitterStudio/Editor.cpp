#include "Editor.h"
#include "File.h"
#include "FileGUI.h"
#include "ResourceManager.h"
#include "GTMEditor.h"
#include "CommandManager.h"
#include "Utilities.h"
#include "Logger.h"
#include <Windows.h>
#include <filesystem>

int Editor::screenWidth = 1366;
int Editor::screenHeight = 768;
EditorSettings Editor::settings;
std::vector<DeltaTime> Editor::times;

Editor::Editor(const std::string& dir) : frameDelta{ 0 }, lastFrame{ 0 }, selectedParent{ -1 }, selectedChild{ -1 }
{
	setDirectory(dir);
	loadSettings(appDir + editorConfig);
	initOpenGl();
	initImgui();
	setImguiStyle();

	ResourceManager::loadShader("BillboardParticle", shadersDir + "BillboardParticle");
	ResourceManager::loadShader("MeshParticle", shadersDir + "MeshParticle");
	ResourceManager::loadShader("Grid", shadersDir + "GridShader");
	ResourceManager::loadShader("Mesh", shadersDir + "Mesh");

	inspector = new Inspector();
	player = new GlitterPlayer();
	inputManager = new InputManager();
	modelViewer = new ModelViewer();
	textureViewer = new TextureViewer();
	renderer = new Renderer();

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
	times.emplace_back(DeltaTime(name, t.elapsed()));
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

		std::vector<std::shared_ptr<MaterialNode>> materials = GTMEditor::getNodes();
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
		std::vector<std::shared_ptr<MaterialNode>> materials = GTMEditor::getNodes();
		for (const auto& m : materials)
		{
			if (m->getMaterial()->getFilename() == filename)
				return false;
		}

		auto material = std::make_shared<Glitter::GlitterMaterial>(filename);
		auto materialNode = std::make_shared<MaterialNode>(material);
		GTMEditor::add(materialNode);
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
	int matCount = 0;
	for (const auto& file : std::filesystem::directory_iterator(directory))
	{
		std::string extension = file.path().extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if (extension == ".gte")
			files.emplace_back(file.path().string());
		else if (extension == ".gtm")
			++matCount;
	}

	Logger::log(Message(MessageType::Normal, "Found " + std::to_string(files.size()) + " GTE files."));

	effectNodes.reserve(effectNodes.size() + files.size());
	GTMEditor::reserve(matCount);

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
	GTMEditor::clean();
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
			auto materials = GTMEditor::getNodes();
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
	std::vector<std::shared_ptr<MaterialNode>> nodes = GTMEditor::getNodes();
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

	sprintf(buffer, "%d.%d.%d", major, minor, rev);
}

void Editor::loadSettings(const std::string& filename)
{
	if (!std::filesystem::exists(filename))
		return;

	Glitter::BinaryReader reader(filename, Glitter::Endianness::LITTLE);
	
	settings.vsync = reader.readChar();
	settings.colorWheel = reader.readChar();
	settings.matPreview = reader.readChar();
	settings.fpsCounter = reader.readChar();
	
	settings.windowSize.x = reader.readSingle();
	settings.windowSize.y = reader.readSingle();
	settings.maximized = reader.readChar();

	settings.particlesOpen = reader.readChar();
	settings.statsOpen = reader.readChar();
	settings.historyViewOpen = reader.readChar();

	reader.close();
}

void Editor::saveSettings(const std::string& filename)
{
	Glitter::BinaryWriter writer(filename, Glitter::Endianness::LITTLE);

	writer.writeChar(settings.vsync);
	writer.writeChar(settings.colorWheel);
	writer.writeChar(settings.matPreview);
	writer.writeChar(settings.fpsCounter);

	writer.writeSingle(settings.windowSize.x);
	writer.writeSingle(settings.windowSize.y);

	settings.maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
	writer.writeChar(settings.maximized);

	writer.writeChar(settings.particlesOpen);
	writer.writeChar(settings.statsOpen);
	writer.writeChar(settings.historyViewOpen);

	writer.close();
}

void Editor::setDirectory(const std::string& dir)
{
	appDir = Glitter::File::getFilePath(dir);
	shadersDir = appDir + "Res\\Shaders\\";
	fontsDir = appDir + "Res\\Fonts\\";
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

	inputManager->update(window);
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
				saveMaterial(GTMEditor::getSelection(), false);
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
			if (selectedParent > -1)
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
	GTMEditor::clear();
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

		processInput();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		resizeLayout(dockspaceID, screenWidth, screenHeight);
		initLayout(dockspaceID, screenWidth, screenHeight);

		if (settings.imguiDemoOpen)
			ImGui::ShowDemoWindow(&settings.imguiDemoOpen);

		updateGlitterTreeView();
		GTMEditor::update();
		inspector->update();
		player->update(renderer, frameDelta);
		modelViewer->update(renderer, frameDelta);
		textureViewer->update();
		Logger::show();

		logTime("Main Timer", mainTimer);

		debugInfo();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		glfwSwapBuffers(window);
	}

	saveSettings(appDir + editorConfig);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

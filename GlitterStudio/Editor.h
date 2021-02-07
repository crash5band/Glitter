#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "Inspector.h"
#include "GlitterPlayer.h"
#include "InputManager.h"
#include "UIFn.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

struct EditorSettings
{
	// Editor
	bool vsync = true;
	bool colorWheel = false;
	bool matPreview = true;
	bool fpsCounter = false;

	// Window
	Glitter::Vector2 windowSize{ 1366, 768 };
	bool maximized = false;

	// Editor Windows
	bool particlesOpen = false;
	bool statsOpen = false;
	bool historyViewOpen = false;
	bool imguiDemoOpen = false;
};

struct DeltaTime
{
	const char* name;
	double time;

	DeltaTime(const char* n, double t) :
		name{ n }, time{ t }
	{

	}
};

class Editor
{
	using effPtr = std::shared_ptr<EffectNode>;
	using emPtr = std::shared_ptr<EmitterNode>;
	using pPtr = std::shared_ptr<ParticleNode>;

private:
	static int screenWidth;
	static int screenHeight;
	const char* glslVersion = "#version 130";

	GLFWwindow* window;
	ImGuiIO *io;
	ImGuiID dockspaceID;

	std::string shadersDir;
	std::string fontsDir;
	std::string imguiConfig = "imgui.ini";
	std::string editorConfig = "config.dat";

	std::string appDir;
	std::vector<effPtr> effectNodes;
	std::vector <std::weak_ptr<ParticleNode>> availableParticles;
	Inspector* inspector;
	GlitterPlayer* player;
	InputManager* inputManager;
	Stopwatch mainTimer;
	int selectedParent;
	int selectedChild;

	float lastFrame;
	float frameDelta;

	bool initOpenGl();
	bool initImgui();
	void setImguiStyle();
	void debugInfo();

public:
	Editor(const std::string& dir);

	static std::vector<DeltaTime> times;
	static EditorSettings settings;
	static void setScreenDimensions(int width, int height);
	static void logTime(const char* name, const Stopwatch& t);

	void go();
	void reset();
	bool openGlitterFile(const std::string& filename);
	void openFolder(const std::string& directory);
	void closeEffect(int index);
	void closeAllEffects();
	void cleanUp();
	void saveEffect(int index, bool saveAs);
	void saveMaterial(int index, bool saveAs);
	void about();
	bool isNodeSelected(int index1, int index2);
	void setNodeSelected(int p, int c);
	void setSelectedNode();
	void processInput();
	void updateGlitterTreeView();
	void updateMenuBar();
	void frameTime();
	void getVersion(char* buffer);
	void setDirectory(const std::string& dir);
	void loadSettings(const std::string& filename);
	void saveSettings(const std::string& filename);

	bool effectMenu(int index);
	bool emitterMenu(int parent, int index);
	bool particleMenu(int parent, int index);
	bool instanceMenu(int effect, int parent, int index);
	void createEffect(const std::string& name);
	void createMaterial(const std::string& name);
	void createEmitter(effPtr eff, const std::string& name);
	void createParticle(effPtr eff, const std::string& name);
	void removeEmitter(effPtr eff, size_t index);
	void removeParticle(effPtr eff, size_t index);
	void addParticleInstance(emPtr em, pPtr p, size_t pos);
	void removeParticleInstance(emPtr em, pPtr, size_t index);
	std::vector<std::weak_ptr<ParticleNode> > getAvailableParticles(effPtr eff, emPtr em);
};

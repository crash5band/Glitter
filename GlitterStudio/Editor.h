#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "Inspector.h"
#include "GlitterPlayer.h"
#include "MaterialEditor.h"
#include "CommandManager.h"
#include "InputManager.h"
#include "UIFn.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

struct DebugWindows
{
	bool particlesOpen = false;
	bool performanceOpen = false;
	bool historyViewOpen = false;
};

struct EditorSetting
{
	bool vsync;
	bool colorWheel;
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
	using pPtr = std::shared_ptr<Glitter::Particle>;

private:
	static int screenWidth;
	static int screenHeight;
	const char* glslVersion = "#version 130";

	GLFWwindow* window;
	ImGuiIO *io;
	ImGuiID dockspaceID;

	std::vector<std::shared_ptr<EffectNode>> effectNodes;
	std::vector <std::weak_ptr<Glitter::Particle>> availableParticles;
	Inspector* inspector;
	GlitterPlayer* player;
	InputManager* inputManager;
	int selectedParent;
	int selectedChild;
	int showEffect;
	int showEmitter;
	bool showAvailableParticles;

	float lastFrame;
	float frameDelta;

	bool initOpenGl();
	bool initImgui();
	void setImguiStyle();
	void debugInfo();

public:
	Editor();

	static bool resizing;
	static DebugWindows debugWindows;
	static EditorSetting editorSettings;
	static std::vector<DeltaTime> times;
	static void setScreenDimensions(int width, int height);

	void go();
	void reset();
	bool openGlitterFile(const std::string& filename);
	void saveEffect(bool saveAs);
	void saveMaterial(bool saveAs);
	bool isNodeSelected(int index1, int index2);
	void setNodeSelected(int p, int c);
	void setSelectedNode();
	void processInput();
	void updateGlitterTreeView();
	void updateMenuBar();
	void frameTime();

	void effectMenu(int index);
	void emitterMenu(int parent, int index);
	void particleMenu(int parent, int index);
	void instanceMenu(int effect, int parent, int index);
	void availableParticlesMenu(int parent, int index);
	void createEffect(const std::string& name);
	void createMaterial(const std::string& name);
	void createEmitter(effPtr eff, const std::string& name);
	void createParticle(effPtr eff, const std::string& name);
	void removeEmitter(effPtr eff, size_t index);
	void removeParticle(effPtr eff, size_t index);
	unsigned int firstAvailableID(std::vector<unsigned int>& idList);
	void addParticleInstance(emPtr em, std::shared_ptr<ParticleNode> p, size_t pos);
	void removeParticleInstance(emPtr em, std::shared_ptr<ParticleNode> p, size_t index);
	std::vector<std::weak_ptr<Glitter::Particle> > getAvailableParticles(effPtr eff, emPtr em);
};

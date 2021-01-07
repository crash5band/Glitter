#include "Editor.h"
#include "IconsFontAwesome5.h"
#include <stdio.h>
#include <Windows.h>
#include <sysinfoapi.h>

void frameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	if (width && height)
		Editor::setScreenDimensions(width, height);
}

bool Editor::initOpenGl()
{
	// GLFW initializion
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(1366, 768, "Glitter Studio", NULL, NULL);
	if (window == NULL)
	{
		printf("FATAL: failed to create opengl window\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	// GLAD initializtion
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("FATAL: failed to fetch gl procedures\n");
		return false;
	}

	glViewport(0, 0, 1366, 768);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	glfwSetWindowSizeLimits(window, 1366, 768, GLFW_DONT_CARE, GLFW_DONT_CARE);
	//glfwSetWindowAspectRatio(window, 16, 9);
	//glDisable(GL_CULL_FACE);

	return true;
}

bool Editor::initImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();

	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
	io->ConfigWindowsMoveFromTitleBarOnly = true;
	io->IniFilename = imguiINIDir.c_str();

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImFontConfig fontConfig;
	fontConfig.MergeMode = true;
	fontConfig.GlyphMinAdvanceX = 13.0f;
	static const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

	TCHAR windir[MAX_PATH];
	GetWindowsDirectoryA(windir, MAX_PATH);

	std::string fontDir = windir;
	fontDir.append("/Fonts/segoeui.ttf");

	io->Fonts->AddFontFromFileTTF(fontDir.c_str(), 18, NULL, io->Fonts->GetGlyphRangesDefault());
	io->Fonts->AddFontFromFileTTF(std::string(appDir + "/Res/Fonts/fa-solid-900.ttf").c_str(), 15.0f, &fontConfig, iconRanges);

	return true;
}

void Editor::setImguiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->FramePadding.x = 5;
	style->FramePadding.y = 3;
	style->ItemSpacing.x = 10;
	style->ItemSpacing.y = 3;
	style->WindowPadding.x = 5;
	style->WindowRounding = 0;
	style->WindowBorderSize = 1;
	style->FrameBorderSize = 0;
	style->FrameRounding = 1.5f;
	style->ScrollbarRounding = 4.0f;
	style->GrabRounding = 2.0f;
	style->TabRounding = 2;
	style->ScrollbarSize = 16;

	ImVec4* colors = style->Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.1f, 0.1f, 0.1f, 0.85f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.10f, 0.10f, 0.10f, 0.35f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.15f, 0.15f, 0.78f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.44f, 0.75f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.16f, 0.44f, 0.75f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.48f, 0.88f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.44f, 0.75f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.19f, 0.48f, 0.88f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.16f, 0.44f, 0.75f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
}

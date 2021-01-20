#include "GlitterPlayer.h"
#include "Editor.h"
#include "IconsFontAwesome5.h"
#include "UiHelper.h"
#include "ResourceManager.h"

constexpr float windowOffset = 73.0f;

GlitterPlayer::GlitterPlayer() :
	firstFrame{ 0 }, lastFrame{ 0 }, time{ 0.0f }, playbackSpeed{ 1.0f }, playing{ false }, loop{ true }, mouseInViewArea{ false }
{
	selectedEffect = nullptr;
	preview = new RenderTarget(1920, 1080);
	camera = new Camera(CameraMode::Orbit);
	renderer = new Renderer();
}

GlitterPlayer::~GlitterPlayer()
{
	delete preview;
	delete camera;
	delete renderer;
}

void GlitterPlayer::togglePlayback()
{
	if (selectedEffect)
		playing ^= true;
}

void GlitterPlayer::stopPlayback()
{
	playing = false;
	time = 0.0f;

	if (selectedEffect)
		selectedEffect->kill();
}

void GlitterPlayer::setEffect(EffectNode* node)
{
	if (node != selectedEffect)
	{
		selectedEffect = node;

		if (node)
			selectedEffect->buildAnimations();

		stopPlayback();
		togglePlayback();
		camera->reset();
	}
}

bool GlitterPlayer::isEffectLoop()
{
	return selectedEffect->getEffect()->getFlags() & 1;
}

void GlitterPlayer::updatePreview(float deltaT)
{
	viewArea = ImRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowSize().x,
		ImGui::GetCursorScreenPos().y + ImGui::GetWindowSize().y - windowOffset));

	mouseInViewArea = ImGui::IsMouseHoveringRect(viewArea.Min, viewArea.Max);
	if (mouseInViewArea && ImGui::IsWindowFocused())
	{
		ImGuiIO &io = ImGui::GetIO();
		camera->update(io.MouseDown[0], io.MouseDown[1], io.MouseDelta.x, io.MouseDelta.y, io.MouseWheel);
	}

	preview->use();
	preview->clear();
	if (selectedEffect)
	{
		firstFrame = selectedEffect->getEffect()->getStartTime();
		lastFrame = firstFrame + selectedEffect->getEffect()->getLifeTime() + 60.0f;
		if (time > lastFrame && !isEffectLoop())
		{
			stopPlayback();
			if (loop)
				togglePlayback();
		}

		timer.reset();

		selectedEffect->update(time, camera);
		time += deltaT * 60.0f * playbackSpeed * playing;

		Editor::logTime("Update Effect", timer);
		timer.reset();

		Glitter::Vector2 size(viewArea.Max.x - viewArea.Min.x, viewArea.Max.y - viewArea.Min.y);
		renderer->drawEffect(selectedEffect, camera, size);

		Editor::logTime("Draw Effect", timer);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	auto texture = preview->getTexture();
	ImGui::Image((void*)texture, ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - windowOffset), ImVec2(1, 1), ImVec2(0, 0),
		ImVec4(1, 1, 1, 1), ImVec4(0.1, 0.1, 0.1, 0.85));
}

void GlitterPlayer::update(float deltaT)
{
	if (ImGui::Begin(prevWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		if (ImGui::Button(ICON_FA_VIDEO " Reset", ImVec2(80, btnHeight)))
			camera->reset();

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_STOP, btnNormal))
			stopPlayback();

		ImGui::SameLine();
		if (ImGui::Button(playing ? ICON_FA_PAUSE : ICON_FA_PLAY, btnWide))
			togglePlayback();

		ImGui::SameLine();
		ImGui::Checkbox("Loop", &loop);

		static const char* speedsChar[]{"0.25x", "0.50x", "0.75x", "1.00x"};
		static float speeds[]{ 0.25f, 0.5f, 0.75f, 1.0f };
		int selectedIndex = (playbackSpeed * 4) - 1;

		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 4.0f);
		if (ImGui::BeginCombo("Playback Speed", speedsChar[selectedIndex]))
		{
			for (int i = 0; i < 4; ++i)
			{
				const bool selected = selectedIndex == i;
				if (ImGui::Selectable(speedsChar[i], selected))
				{
					selectedIndex = i;
					playbackSpeed = speeds[i];
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem(playing ? "Pause" : "Play"))
				togglePlayback();

			if (ImGui::MenuItem("Stop"))
				stopPlayback();

			if (ImGui::MenuItem("Loop", NULL, loop))
				loop ^= true;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		ImGui::Text("Camera Pitch: %.2f, Yaw: %.2f", camera->getPitch(), camera->getYaw());
		
		updatePreview(deltaT);
	}
	ImGui::End();
}

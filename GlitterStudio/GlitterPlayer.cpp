#include "GlitterPlayer.h"
#include "Editor.h"
#include "IconsFontAwesome5.h"
#include "ResourceManager.h"
#include "Stopwatch.h"

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

		{
			Stopwatch updateTimer("Update Effect");
			selectedEffect->update(time, camera);
		}

		time += deltaT * 60.0f * playbackSpeed * playing;

		{
			Stopwatch drawTimer("Draw Effect");
			Glitter::Vector2 size(viewArea.Max.x - viewArea.Min.x, viewArea.Max.y - viewArea.Min.y);
			renderer->drawEffect(selectedEffect, camera, size);
		}
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
		//ImGui::Button(ICON_FA_VIDEO, ImVec2(30, 25));

		if (ImGui::Button(ICON_FA_VIDEO " Reset", ImVec2(80, 25)))
			camera->reset();

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_STOP, ImVec2(30, 25)))
			stopPlayback();

		ImGui::SameLine();
		if (ImGui::Button(playing ? ICON_FA_PAUSE : ICON_FA_PLAY, ImVec2(50, 25)))
			togglePlayback();

		ImGui::SameLine();
		ImGui::Checkbox("Loop", &loop);

		static const char* speeds[2]{ "0.50", "1.00"};
		int selectedIndex = (int)(playbackSpeed / 0.5f) - 1;
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 4.0f);
		if (ImGui::BeginCombo("Playback Speed", speeds[selectedIndex]))
		{
			for (int i = 0; i < 2; ++i)
			{
				const bool selected = selectedIndex == i;
				if (ImGui::Selectable(speeds[i], selected))
				{
					selectedIndex = i;
					playbackSpeed = (i + 1) * 0.5f;
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
		ImGui::Text("Camera Pitch: %.3f, Yaw: %.3f", camera->getPitch(), camera->getYaw());
		
		updatePreview(deltaT);
	}
	ImGui::End();
}

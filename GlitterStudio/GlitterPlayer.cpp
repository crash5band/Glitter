#include "GlitterPlayer.h"
#include "Editor.h"
#include "IconsFontAwesome5.h"
#include "UiHelper.h"
#include "ResourceManager.h"

GlitterPlayer::GlitterPlayer() :
	playbackSpeed{ 1.0f }, playing{ false }, loop{ true }, drawGrid{ true }, mouseInViewArea{ false }
{
	time = maxTime	= 0;
	selectedEffect	= nullptr;
	preview			= new RenderTarget(1920, 1080);
	camera			= new Camera(CameraMode::Orbit);
	renderer		= new Renderer();
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
	time	= 0.0f;

	if (selectedEffect)
		selectedEffect->kill();
}

void GlitterPlayer::replay()
{
	stopPlayback();
	togglePlayback();
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
		//camera->reset();
	}
}

bool GlitterPlayer::isEffectLoop()
{
	if (!selectedEffect)
		return false;

	return selectedEffect->getEffect()->getFlags() & 1;
}

void GlitterPlayer::updatePreview(float deltaT)
{
	viewArea = ImRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail());
	mouseInViewArea = ImGui::IsMouseHoveringRect(viewArea.Min, viewArea.Max, false);

	if (mouseInViewArea && ImGui::IsWindowFocused())
	{
		ImGuiIO& io = ImGui::GetIO();
		camera->update(io.MouseDown[0], io.MouseDown[1], io.MouseDelta.x, io.MouseDelta.y, io.MouseWheel);
	}

	Glitter::Vector2 size(viewArea.Max.x - viewArea.Min.x, viewArea.Max.y - viewArea.Min.y);

	preview->use();
	preview->clear();

	if (drawGrid)
		renderer->drawGrid(camera, size);

	if (selectedEffect)
	{
		maxTime = selectedEffect->getEffect()->getStartTime() + selectedEffect->getEffect()->getLifeTime() + 60.0f;
		if (time > maxTime && !isEffectLoop())
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

		renderer->drawEffect(selectedEffect, camera, size);
		Editor::logTime("Draw Effect", timer);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	auto texture = preview->getTexture();
	ImGui::Image((void*)texture, ImGui::GetContentRegionAvail(), ImVec2(1, 1), ImVec2(0, 0),
		ImVec4(1, 1, 1, 1), ImVec4(0.1, 0.1, 0.1, 0.85));
}

void GlitterPlayer::update(float deltaT)
{
	if (ImGui::Begin(prevWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
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
		if (ImGui::Button(ICON_FA_REDO_ALT, btnNormal))
			replay();

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

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
		ImGui::Checkbox("Loop", &loop);
		ImGui::SameLine();
		ImGui::Checkbox("Grid", &drawGrid);

		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem(playing ? "Pause" : "Play"))
				togglePlayback();

			if (ImGui::MenuItem("Stop"))
				stopPlayback();

			ImGui::MenuItem("Loop", NULL, &loop);
			ImGui::MenuItem("Show Grid", NULL, &drawGrid);

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
		
		updatePreview(deltaT);
	}
	ImGui::End();
}

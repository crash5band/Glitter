#include "GlitterPlayer.h"
#include "Editor.h"
#include "IconsFontAwesome5.h"
#include "UiHelper.h"
#include "ResourceManager.h"

GlitterPlayer::GlitterPlayer() :
	playbackSpeed{ 1.0f }, playing{ false }, loop{ true }, drawGrid{ true }, drawRefModel{ true }
{
	time = maxTime	= 0;
	selectedEffect	= nullptr;
	renderer		= new Renderer();
}

GlitterPlayer::~GlitterPlayer()
{
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
	viewport.use();

	if (drawGrid)
		renderer->drawGrid(viewport);

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

		selectedEffect->update(time, viewport.getCamera());
		time += deltaT * 60.0f * playbackSpeed * playing;

		Editor::logTime("Update Effect", timer);
		timer.reset();

		std::shared_ptr<ModelData> refModel = selectedEffect->getRefModel();
		if (drawRefModel && refModel)
		{
			renderer->drawMesh(refModel, viewport);
		}

		renderer->drawEffect(selectedEffect, viewport);
		Editor::logTime("Draw Effect", timer);
	}

	viewport.end();
}

void GlitterPlayer::update(float deltaT)
{
	if (ImGui::Begin(prevWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		viewport.cameraControl();

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
		ImGui::SameLine();
		ImGui::Checkbox("Reference", &drawRefModel);

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

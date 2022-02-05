#include "GlitterPlayer.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "IconsFontAwesome5.h"
#include "File.h"
#include "UI.h"
#include "UiHelper.h"

namespace Glitter
{
	namespace Editor
	{
		GlitterPlayer::GlitterPlayer() :
			playbackSpeed{ 1.0f }, playing{ false }, loop{ true }, drawGrid{ true }, playOnSelect{ true }
		{
			time = maxTime = 0;
			selectedEffect = nullptr;
		}

		GlitterPlayer::~GlitterPlayer()
		{

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

		void GlitterPlayer::replay()
		{
			stopPlayback();
			togglePlayback();
		}

		bool GlitterPlayer::isPlaying()
		{
			return playing;
		}

		bool GlitterPlayer::isLoop()
		{
			return loop;
		}

		void GlitterPlayer::setEffect(EffectNode* node)
		{
			if (node != selectedEffect)
			{
				selectedEffect = node;
				stopPlayback();
				
				if (playOnSelect)
					togglePlayback();
			}
		}

		bool GlitterPlayer::isEffectLoop()
		{
			if (!selectedEffect)
				return false;

			return selectedEffect->getEffect()->getFlags() & 1;
		}

		void GlitterPlayer::updatePreview(Renderer* renderer, float deltaT)
		{
			viewport.use();

			if (drawGrid)
				renderer->drawGrid(viewport);

			if (selectedEffect)
			{
				// give extra time for if some particles are still alive
				maxTime = selectedEffect->getEffect()->getStartTime() + selectedEffect->getEffect()->getLifeTime() + 60.0f;
				if (time > maxTime && !isEffectLoop())
				{
					stopPlayback();
					if (loop)
						togglePlayback();
				}

				selectedEffect->update(time, viewport.getCamera());
				time += deltaT * 60.0f * playbackSpeed * playing;

				renderer->drawEffect(selectedEffect, viewport);
			}

			viewport.end();
		}

		void GlitterPlayer::update(Renderer* renderer, float deltaT)
		{
			if (ImGui::Begin(UI::gPlayerWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
			{
				viewport.screenshotControl();
				ImGui::SameLine();
				viewport.cameraControl();
				ImGui::SameLine();
				viewport.renderingControl();

				ImGui::SameLine();
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::SameLine();

				if (UI::transparentButton(ICON_FA_STOP, UI::btnNormal))
					stopPlayback();

				ImGui::SameLine();
				if (UI::transparentButton(playing ? ICON_FA_PAUSE : ICON_FA_PLAY, UI::btnNormal))
					togglePlayback();

				ImGui::SameLine();
				if (UI::transparentButton(ICON_FA_REDO_ALT, UI::btnNormal))
					replay();

				ImGui::SameLine();
				ImGui::SetNextItemWidth(150);
				ImGui::SliderFloat("Preview speed", &playbackSpeed, 0.1f, 2.0f, "%.2f");
				playbackSpeed = std::clamp(playbackSpeed, 0.1f, 2.0f);

				if (ImGui::BeginPopupContextItem("speed_context_menu"))
				{
					if (ImGui::MenuItem("Reset"))
						playbackSpeed = 1.0f;

					ImGui::EndPopup();
				}

				ImGui::SameLine();
				ImGui::Checkbox("Loop", &loop);

				ImGui::SameLine();
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::SameLine();
				ImGui::Checkbox("Grid", &drawGrid);
				
				ImGui::BeginMainMenuBar();
				if (ImGui::BeginMenu("View"))
				{
					ImGui::MenuItem("Play On Select", NULL, &playOnSelect);

					if (ImGui::MenuItem(playing ? "Pause" : "Play"))
						togglePlayback();

					if (ImGui::MenuItem("Stop"))
						stopPlayback();

					if (ImGui::MenuItem("Replay"))
						replay();

					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();

				updatePreview(renderer, deltaT);
			}
			ImGui::End();
		}
	}
}

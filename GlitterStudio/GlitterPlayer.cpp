#include "GlitterPlayer.h"
#include "ResourceManager.h"
#include "IconsFontAwesome5.h"
#include "File.h"
#include "UI.h"
#include "UiHelper.h"

namespace Glitter
{
	namespace Editor
	{
		GlitterPlayer::GlitterPlayer() :
			playbackSpeed{ 1.0f }, playing{ false }, loop{ true }, drawGrid{ true }, drawRefModel{ true }, playOnSelect{ true }, showTime{ false }
		{
			time = maxTime = 0;
			selectedEffect = nullptr;
			refModel = nullptr;
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

		void GlitterPlayer::stepFrame()
		{
			if (selectedEffect && !playing)
				time += 1;
		}

		bool GlitterPlayer::isPlaying()
		{
			return playing;
		}

		bool GlitterPlayer::isLoop()
		{
			return loop;
		}

		void GlitterPlayer::changeRefMesh(const std::string& path)
		{
			if (!path.size())
				refModel = nullptr;

			ResourceManager::loadModel(path);
			refModel = ResourceManager::getModel(Glitter::File::getFileName(path));
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

			if (drawRefModel && refModel)
			{
				renderer->drawMesh(refModel, viewport, time);
			}

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

			if (ImGui::BeginPopupContextItem("viewport_popup"))
			{
				if (ImGui::MenuItem("Reset Camera"))
					viewport.resetCamera();

				ImGui::Separator();
				if (ImGui::MenuItem(playing ? "Pause" : "Play"))
					togglePlayback();

				if (ImGui::MenuItem("Stop"))
					stopPlayback();

				if (!playing)
				{
					if (ImGui::MenuItem("Step Frame"))
						stepFrame();
				}

				ImGui::MenuItem("Loop", NULL, &loop);

				if (ImGui::BeginMenu("Playback speed"))
				{
					static const char* speeds[] = { "0.25x", "0.50x", "0.75x", "1.00x" };
					for (int i = 1; i <= 4; ++i)
					{
						if (ImGui::MenuItem(speeds[i - 1], NULL, playbackSpeed == (0.25 * i)))
							playbackSpeed = i * 0.25;
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}
		}

		void GlitterPlayer::update(Renderer* renderer, float deltaT)
		{
			if (ImGui::Begin(UI::gPlayerWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
			{
				viewport.screenshotControl();
				ImGui::SameLine();
				viewport.cameraControl();
				ImGui::SameLine();
				viewport.lightControl();
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

				if (showTime)
				{
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();

					ImGui::Text("Time: %.2f", time);

					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();
				}

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
				
				/*
				ImGui::SameLine();
				ImGui::Checkbox("Reference", &drawRefModel);
				ImGui::SameLine();

				if (ImGui::Button(refModel ? refModel->getName().c_str() : "None", ImVec2(175, UI::btnHeight)))
				{
					std::string name;
					if (FileGUI::openFileGUI(FileType::Model, name))
						changeRefMesh(name);
				}

				ImGui::SameLine();
				if (UI::transparentButton(ICON_FA_TIMES, UI::btnNormal))
					changeRefMesh("");

				*/
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

					ImGui::Separator();
					ImGui::MenuItem("Show Effect Time", NULL, &showTime);

					ImGui::MenuItem("Loop", NULL, &loop);
					ImGui::Separator();

					ImGui::MenuItem("Show Grid", NULL, &drawGrid);

					if (ImGui::MenuItem("Lights", NULL, viewport.isLightEnabled()))
						viewport.toggleLight();

					if (ImGui::MenuItem("Reset Camera"))
						viewport.resetCamera();

					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();

				updatePreview(renderer, deltaT);
			}
			ImGui::End();
		}
	}
}

#include "AnimationTimeline.h"
#include "UI.h"
#include "Utilities.h"
#include "AnimationCommands.h"
#include "CommandManager.h"
#include "IconsFontAwesome5.h"
#include "Logger.h"
#include <algorithm>

namespace Glitter
{
	namespace Editor
	{
		AnimationTimeline::AnimationTimeline(const char* n) :
			minZoom{ 1.0f }, maxZoom{ 5.0f }, frameWidth{ 8 }, keyRadiusIn{ 6.0f }, keyRadiusOut{ 8.0f }, heightOffset{ 20.0f }
		{
			name = n;
			zoom = 2.5f;
			holdingTan = false;
			showKeyTooltip = false;
			canPaste = false;
			frameStart = frameEnd = currentFrame = 0;
			timelineMinOffset = ImVec2(-100, 0);
			timelinePosOffset = ImVec2(timelineMinOffset.x, 0);

			mode = TimelineMode::CurveEditor;

			animationIndex = -1;
			limitIndex = -1;
			initLimits();
		}

		void AnimationTimeline::initLimits()
		{
			for (int limit = 0; limit < 6; ++limit)
			{
				switch (limit)
				{
				case 0:
					higherLimits[limit] = 50;
					break;

				case 1:
					higherLimits[limit] = 360;
					break;

				case 2:
					higherLimits[limit] = 10;
					break;

				case 3:
					higherLimits[limit] = 2;
					break;

				case 4:
					higherLimits[limit] = 255;
					break;

				case 5:
					higherLimits[limit] = 100;
					break;

				default:
					higherLimits[limit] = 0;
					break;
				}

				lowerLimits[limit] = limit < 4 ? -higherLimits[limit] : 0;
			}
		}

		int AnimationTimeline::getCurrentFrame() const
		{
			return currentFrame;
		}

		int AnimationTimeline::getSelectedKey() const
		{
			return selectedKey;
		}

		void AnimationTimeline::setAnimations(std::shared_ptr<EditorAnimationSet> set)
		{
			animSet = set;
		}

		std::vector<std::string> AnimationTimeline::getAvailableAnimations() const
		{
			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			std::vector<std::string> available;
			if (!set)
				return available;

			available.reserve(animationTypeTableSize);

			for (int a = 0; a < animationTypeTableSize; ++a)
			{
				bool found = false;
				for (int b = 0; b < set->animations.size(); ++b)
				{
					if (animationTypeTable[a] == set->animations[b].name)
					{
						found = true;
						break;
					}
				}

				if (!found)
					available.push_back(animationTypeTable[a]);
			}

			return available;
		}

		void AnimationTimeline::updateAnimationControls()
		{
			std::shared_ptr<EditorAnimationSet> set = animSet.lock();

			ImGui::PopStyleVar();
			UI::transparentButton(ICON_FA_PLUS, UI::btnNormal);
			if (set)
			{
				if (ImGui::BeginPopupContextItem("available_anims", ImGuiPopupFlags_MouseButtonLeft))
				{
					std::vector<std::string> availableAnims = getAvailableAnimations();

					ImGui::Text("Available Animations");
					ImGui::Separator();

					for (int i = 0; i < availableAnims.size(); ++i)
					{
						if (ImGui::Selectable(availableAnims[i].c_str()))
						{
							int typeIndex = glitterStringToEnum(animationTypeTable, animationTypeTableSize, availableAnims[i]);
							EditorAnimation newAnim(GlitterAnimation((AnimationType)typeIndex, 0.0f));
							CommandManager::pushNew(new AddAnimationCommand(set, newAnim, set->animations.size()));
						}
					}

					ImGui::EndPopup();
				}
			}

			ImGui::SameLine();
			if (UI::transparentButton(ICON_FA_TRASH, UI::btnNormal))
			{
				if (set)
				{
					if (animationIndex > -1 && animationIndex < set->animations.size())
					{
						CommandManager::pushNew(new RemoveAnimationCommand(set, animationIndex));
						animationIndex = std::clamp(animationIndex, -1, (int)set->animations.size() - 1);
					}
				}
			}

			ImGui::SameLine();
			timelineSettings();
		}

		void AnimationTimeline::timelineSettings()
		{
			UI::transparentButton(ICON_FA_WRENCH, UI::btnNormal);
			if (ImGui::BeginPopupContextItem("timeline_settings", ImGuiPopupFlags_MouseButtonLeft))
			{
				ImGui::Checkbox("Show key tooltip", &showKeyTooltip);
				ImGui::Separator();

				const bool disabled = limitIndex == -1;
				if (disabled)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				float lim[2]{ disabled ? 0 : lowerLimits[limitIndex], disabled ? 1 : higherLimits[limitIndex] };
				if (ImGui::DragFloat2("Lower/Higher Limits", lim, 1.0f, 0.0f, 0.0f, "%.2f"))
				{
					if (lim[0] < lim[1] && limitIndex >= 0 && limitIndex < 6)
					{
						lowerLimits[limitIndex] = lim[0];
						higherLimits[limitIndex] = lim[1];
					}
				}

				if (disabled)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::EndPopup();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 3));
		}

		void AnimationTimeline::timelineContextMenu()
		{
			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			if (!set)
				return;

			if (animationIndex < 0 || animationIndex >= set->animations.size())
				return;

			if (ImGui::BeginPopupContextWindow("timleine_context_menu"))
			{
				if (ImGui::MenuItem("Set Animation Start"))
				{
					EditorAnimation a = set->animations[animationIndex];
					a.setStart(currentFrame);

					if (a.getStart() != set->animations[animationIndex].getStart())
						CommandManager::pushNew(new ChangeAnimationCommand(set, a, animationIndex));
				}

				if (ImGui::MenuItem("Set Animation End"))
				{
					EditorAnimation a = set->animations[animationIndex];
					a.setEnd(currentFrame);

					if (a.getEnd() != set->animations[animationIndex].getEnd())
						CommandManager::pushNew(new ChangeAnimationCommand(set, a, animationIndex));
				}

				if (ImGui::BeginMenu("Repeat Type"))
				{
					EditorAnimation a = set->animations[animationIndex];
					RepeatType repeat = a.repeat;

					if (ImGui::MenuItem("Constant", NULL, a.repeat == RepeatType::Constant))
						a.repeat = RepeatType::Constant;
					if (ImGui::MenuItem("Repeat", NULL, a.repeat == RepeatType::Repeat))
						a.repeat = RepeatType::Repeat;

					if (a.repeat != repeat)
						CommandManager::pushNew(new ChangeAnimationCommand(set, a, animationIndex));

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (selectedKey == -1)
				{
					if (currentFrame >= set->animations[animationIndex].getStart() && currentFrame <= set->animations[animationIndex].getEnd())
					{
						if (ImGui::MenuItem("Add Key"))
							CommandManager::pushNew(new AddKeyCommand(set, animationIndex, currentFrame));

						if (ImGui::MenuItem("Paste"))
							pasteKey();
					}
				}
				else
				{
					if (ImGui::MenuItem("Copy"))
						copyKey();

					if (ImGui::BeginMenu("Interpolation"))
					{
						EditorKeyframe k = set->animations[animationIndex].keys[selectedKey];
						InterpolationType current = k.interpolation;
						if (ImGui::MenuItem("Linear", NULL, current == InterpolationType::Linear))
							k.interpolation = InterpolationType::Linear;
						if (ImGui::MenuItem("Hermite", NULL, current == InterpolationType::Hermite))
							k.interpolation = InterpolationType::Hermite;
						if (ImGui::MenuItem("Constant", NULL, current == InterpolationType::Constant))
							k.interpolation = InterpolationType::Constant;

						if (k.interpolation != current)
							CommandManager::pushNew(new ChangeKeyCommand(set, animationIndex, k, selectedKey));

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Remove Key"))
						CommandManager::pushNew(new RemoveKeyCommand(set, animationIndex, selectedKey));
				}

				ImGui::EndPopup();
			}
		}

		void AnimationTimeline::updateTimleineControls()
		{
			// Zoom //
			float zoomPercent = zoom * 100.0f;

			if (UI::transparentButton(ICON_FA_SEARCH_MINUS, UI::btnNormal))
				zoomPercent -= 50.0f;

			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 2.0f));
			ImGui::SetNextItemWidth(125.0f);
			ImGui::SliderFloat("##zoom_slider", &zoomPercent, minZoom * 100.0f, maxZoom * 100.0f, "%.1f%%");

			ImGui::SameLine();
			if (UI::transparentButton(ICON_FA_SEARCH_PLUS, UI::btnNormal))
				zoomPercent += 50.0f;

			zoom = zoomPercent / 100.0f;

			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();

			// Cursor Control //
			if (UI::transparentButton(ICON_FA_FAST_BACKWARD, UI::btnNormal))
			{
				currentFrame = frameStart;
				timelinePosOffset.x = ((currentFrame - frameStart) * effectiveFrameWidth) - 100;
			}

			ImGui::SameLine();
			ImGui::PushButtonRepeat(true);
			if (UI::transparentButton(ICON_FA_BACKWARD, UI::btnNormal))
			{
				--currentFrame;
				float curPos = ((currentFrame - frameStart) * effectiveFrameWidth);
				if (timelinePosOffset.x + 100 > curPos)
					timelinePosOffset.x = curPos - 100;
			}

			ImGui::SameLine();
			if (UI::transparentButton(ICON_FA_FORWARD, UI::btnNormal))
			{
				++currentFrame;
				float curPos = ((currentFrame - frameStart) * effectiveFrameWidth);
				if (curPos > timelinePosOffset.x + canvasSize.x - 100)
					timelinePosOffset.x = curPos - (canvasSize.x - 100);
			}

			ImGui::PopButtonRepeat();
			ImGui::SameLine();
			if (UI::transparentButton(ICON_FA_FAST_FORWARD, UI::btnNormal))
			{
				currentFrame = frameEnd;
				timelinePosOffset.x = ((currentFrame - frameStart) * effectiveFrameWidth) - 100;
			}

			if (timelinePosOffset.x < timelineMinOffset.x)
				timelinePosOffset.x = timelineMinOffset.x;

			if (currentFrame < 0)
				currentFrame = 0;
			zoom = std::clamp(zoom, minZoom, maxZoom);

			// Key Creation/Deletion //
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();

			if (UI::transparentButton(ICON_FA_PLUS_CIRCLE, UI::btnNormal))
				newKey();

			ImGui::SameLine();
			if (UI::transparentButton(ICON_FA_MINUS_CIRCLE, UI::btnNormal))
				deleteKey();

			ImGui::SameLine();
			if (UI::transparentButton(ICON_FA_COPY, UI::btnNormal))
				copyKey();

			ImGui::SameLine();
			if (UI::transparentButton(ICON_FA_PASTE, UI::btnNormal))
				pasteKey();
			
			ImGui::PopStyleVar(1);
		}

		void AnimationTimeline::animationProperties()
		{
			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			bool disabled = true;
			if (set)
			{
				disabled = animationIndex < 0 || animationIndex >= set->animations.size();
			}

			ImGui::BeginChild("anim_treeview", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.5f), true);
			UI::itemRowsBackground();

			if (set)
			{
				for (int a = 0; a < set->animations.size(); ++a)
				{
					std::string lbl = Glitter::glitterEnumToString(Glitter::animationTypeTable, Glitter::animationTypeTableSize, (int)set->animations[a].type);
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;

					if (animationIndex == a)
						flags |= ImGuiTreeNodeFlags_Selected;
					ImGui::TreeNodeEx(lbl.c_str(), flags);

					if (ImGui::IsItemActivated())
						animationIndex = a;
				}
			}

			ImGui::EndChild();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 2));
			if (disabled)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			ImGui::BeginChild("anim_property", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);
			ImGui::Text("Animation Properties");
			ImGui::Separator();
			ImGui::BeginColumns("anim_property_col", 2, ImGuiOldColumnFlags_NoBorder);
			ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x / 3);
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

			float dummy1 = 0.0f;
			int dummy2 = 0;

			EditorAnimation a;
			if (!disabled)
				a = set->animations[animationIndex];

			bool edit = false;

			float start = a.getStart();
			float end = a.getEnd();
			int flags = a.getFlags();
			RepeatType repeat = a.repeat;

			ImGui::Text("Start");
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputFloat("##start", &start, 1, 5, "%g", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				edit = true;
				a.setStart(start);
			}
			ImGui::NextColumn();

			ImGui::Text("End");
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputFloat("##end", &end, 1, 5, "%g", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				edit = true;
				a.setEnd(end);
			}
			ImGui::NextColumn();

			ImGui::Text("Flags");
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputInt("##flags", &flags, 1, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				edit = true;
				a.setFlags(flags);
			}
			ImGui::NextColumn();

			ImGui::Text("Repeat");
			ImGui::NextColumn();

			int selectedIndex = disabled ? dummy2 : (size_t)repeat;
			std::string comboLabel = Glitter::glitterEnumToString(repeatTypeTable, repeatTypeTableSize, selectedIndex);
			
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::BeginCombo("##repeat", comboLabel.c_str()))
			{
				for (int n = 0; n < repeatTypeTableSize; ++n)
				{
					const bool selected = selectedIndex == n;
					if (ImGui::Selectable(repeatTypeTable[n].c_str(), selected))
					{
						selectedIndex = n;
						a.repeat = (Glitter::RepeatType)selectedIndex;
						edit |= true;
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::PopItemWidth();
			ImGui::EndColumns();

			if (disabled)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			else
			{
				if (edit)
					CommandManager::pushNew(new ChangeAnimationCommand(set, a, animationIndex));
			}

			ImGui::PopStyleVar();
			ImGui::EndChild();
		}

		void AnimationTimeline::keyProperties()
		{
			ImGui::BeginChild("key_properties", ImGui::GetContentRegionAvail(), true);

			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			bool disabled = !set;
			if (set)
			{
				bool invalidAnim = animationIndex < 0 || animationIndex >= set->animations.size();
				disabled |= invalidAnim;

				if (!invalidAnim)
				{
					bool invalidKey = selectedKey < 0 || selectedKey >= set->animations[animationIndex].keys.size();
					disabled |= invalidKey;
				}
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 2));
			if (disabled)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			ImGui::Text("Key Properties");
			ImGui::Separator();

			ImGui::BeginColumns("key_properties", 2, ImGuiOldColumnFlags_NoBorder);
			ImGui::SetColumnWidth(1, ImGui::GetWindowSize().x / 2);
			const char* properties[] = {"Time", "Value", "Interpolation", "InParam", "OutParam", "Random Range"};
			int str = 0;
			float dummy = 0.0f;

			bool edit = false;

			EditorKeyframe k;
			if (!disabled)
				k = set->animations[animationIndex].keys[selectedKey];

			ImGui::Text(properties[str++]);
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputFloat("##time", &k.time, 1, 5, "%g", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				k.time = std::clamp(k.time, set->animations[animationIndex].getStart(), set->animations[animationIndex].getEnd());
				k.time = set->animations[animationIndex].verifyKeyOrder(selectedKey, k.time);
				currentFrame = k.time;
				edit = true;
			}
			ImGui::NextColumn();

			ImGui::Text(properties[str++]);
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			edit |= ImGui::InputFloat("##value", &k.value, 1, 5, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::NextColumn();

			ImGui::Text(properties[str++]);
			ImGui::NextColumn();

			int selectedIndex = disabled ? 0 : (int)k.interpolation;
			std::string comboLabel = Glitter::glitterEnumToString(interpolationTypeTable, interpolationTypeTableSize, selectedIndex);
			
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::BeginCombo("##interpolation", comboLabel.c_str()))
			{
				for (int n = 0; n < interpolationTypeTableSize; ++n)
				{
					const bool selected = selectedIndex == n;
					if (ImGui::Selectable(interpolationTypeTable[n].c_str(), selected))
					{
						selectedIndex = n;
						k.interpolation = (Glitter::InterpolationType)selectedIndex;
						edit |= true;
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			ImGui::NextColumn();

			ImGui::Text(properties[str++]);
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			edit |= ImGui::InputFloat("##in_param", &k.inParam, 0.01, 0.1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::NextColumn();

			ImGui::Text(properties[str++]);
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			edit |= ImGui::InputFloat("##out_param", &k.outParam, 0.01, 0.1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::NextColumn();

			ImGui::Text(properties[str++]);
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			edit |= ImGui::InputFloat("##rnd_rng", &k.randomRange, 1, 5, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::EndColumns();

			if (disabled)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			else
			{
				if (edit && k != set->animations[animationIndex].keys[selectedKey])
					CommandManager::pushNew(new ChangeKeyCommand(set, animationIndex, k, selectedKey));
			}

			ImGui::PopStyleVar();
			ImGui::EndChild();
		}

		void AnimationTimeline::drawTimelineBase()
		{
			ImGui::BeginChild("timeline", ImGui::GetContentRegionAvail(), true, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
			
			ImGui::PopStyleVar();
			updateTimleineControls();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 3));

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			canvasSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
			canvasPos = ImGui::GetCursorScreenPos();
			boundaries = ImRect(canvasPos, canvasPos + canvasSize);

			ImGui::ItemSize(boundaries);
			effectiveFrameWidth = frameWidth * zoom;
			canvasPos -= timelinePosOffset;

			ImU32 bgColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
			ImGui::RenderFrame(boundaries.Min, boundaries.Max, bgColor, true, 1.0f);
			drawList->PushClipRect(boundaries.Min, boundaries.Max, true);

			int start = std::max((timelinePosOffset.x / effectiveFrameWidth) + frameStart, 0.0f);
			int end = ((timelinePosOffset.x + canvasSize.x) / effectiveFrameWidth) + frameStart;

			for (int frame = start; frame <= end; ++frame)
			{
				const float x = canvasPos.x + ((frame - frameStart) * effectiveFrameWidth);
				const float y = canvasPos.y + ((frame & 1) ? 10 : 0) + canvasSize.y;

				const bool boldFrame = frame % 10 == 0 || frame == frameStart || frame == frameEnd;
				const bool validFrame = frame >= frameStart && frame <= frameEnd;

				ImU32 color1 = ImGui::GetColorU32(ImVec4(0.50, 0.50, 0.50, 0.75));
				ImU32 color2 = ImGui::GetColorU32(ImVec4(0.40, 0.40, 0.40, 0.45));
				ImU32 color3 = ImGui::GetColorU32(ImVec4(0.20, 0.20, 0.20, 0.75f));
				ImU32 result = !validFrame ? color3 : boldFrame ? color1 : color2;

				drawList->AddLine(ImVec2(x, y - canvasSize.y), ImVec2(x, y), result, 0.80f);

				if (boldFrame)
					drawList->AddText(ImVec2(x + 3, canvasPos.y), color1, std::to_string(frame).c_str());
			}

			int count = 0;
			while (count < 5)
			{
				const float xPos{ canvasPos.x + timelinePosOffset.x };
				const float yPos{ canvasPos.y + (0.25f * count * (canvasSize.y - heightOffset)) + heightOffset };

				ImU32 lineColor = (count & 1) ? ImGui::GetColorU32(ImVec4(0.35, 0.35, 0.35, 0.45)) : ImGui::GetColorU32(ImVec4(0.45, 0.45, 0.45, 0.75));
				bool last{ count == 4 };
				if (count && !last)
					drawList->AddLine(ImVec2(xPos, yPos), ImVec2(xPos + canvasSize.x, yPos), lineColor, 0.80f);

				float high = 1.0f;
				float low = 0.0f;
				if (limitIndex >= 0 && limitIndex < (int)AnimationGraphType::Max)
				{
					high = higherLimits[limitIndex];
					low = lowerLimits[limitIndex];
				}

				float val = high - (count * 0.25f) * (high - low);
				size_t length = snprintf(NULL, 0, "%.2f", val);
				char* str = (char*)malloc(length + 1);
				snprintf(str, length + 1, "%.2f", val);

				drawList->AddText(ImVec2(xPos + 2, yPos + (last ? -20 : -2)), ImGui::GetColorU32(ImVec4(0.65, 0.65, 0.65, 0.85)), str);
				free(str);

				++count;
			}

			// draw top horizontal line
			drawList->AddLine(ImVec2(canvasPos.x + timelinePosOffset.x, canvasPos.y + heightOffset), ImVec2(canvasPos.x + canvasSize.x + timelinePosOffset.x, canvasPos.y + 20),
				ImGui::GetColorU32(ImVec4(0.5, 0.5, 0.5, 0.85)), 2.0f);

			timelineContextMenu();
		}

		void AnimationTimeline::drawAnimationCurve(const EditorKeyframe& k1, const EditorKeyframe& k2)
		{
			const float x1{ canvasPos.x + ((k1.time - frameStart) * effectiveFrameWidth) };
			const float x2{ canvasPos.x + ((k2.time - frameStart) * effectiveFrameWidth) };

			// drawing curves can be expensive so return early if the two points are out of range
			if (((x1 < canvasPos.x + timelinePosOffset.x) && (x2 < canvasPos.x + timelinePosOffset.x)) ||
				(x1 > canvasPos.x + timelinePosOffset.x + canvasSize.x) && (x2 > canvasPos.x + timelinePosOffset.x + canvasSize.x))
				return;

			static const ImU32 lineColor{ ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75)) };
			const float lineThickness = 2.0f;

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			if (k1.interpolation == Glitter::InterpolationType::Hermite)
			{
				// Hack but it works. Basically calculate the value at each frame and connect the 'points' to form a curve
				ImVec2* points = (ImVec2*)(malloc(sizeof ImVec2 * ((size_t)k2.time - (size_t)k1.time + 1)));

				for (size_t i = k1.time; i <= k2.time; ++i)
					points[i - (size_t)k1.time] = ImVec2(canvasPos.x + ((i - frameStart) * effectiveFrameWidth), valueToHeight(animSet.lock()->animations[animationIndex].interpolate(i, k1, k2)));

				drawList->AddPolyline(points, k2.time - k1.time + 1, lineColor, false, lineThickness);
				free(points);
			}
			else
			{
				const float y1 = valueToHeight(k1.value);
				const float y2 = valueToHeight(k2.value);

				if (k1.interpolation == Glitter::InterpolationType::Linear)
				{
					drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), lineColor, lineThickness);
				}
				else
				{
					// Constant
					drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y1), lineColor, lineThickness);
					drawList->AddLine(ImVec2(x2, y1), ImVec2(x2, y2), lineColor, lineThickness);
				}
			}
		}

		void AnimationTimeline::onHandleDrag(EditorKeyframe& k, int index, ImVec2 &pos)
		{
			currentFrame = k.time;
			selectedKey = index;
			pos.y += ImGui::GetIO().MouseDelta.y;

			if (pos.y < canvasPos.y + 1.0f)
				pos.y = canvasPos.y + 1.0f;
			if (pos.y > canvasPos.y + canvasSize.y - 1.0f)
				pos.y = canvasPos.y + canvasSize.y - 1.0f;
		}

		void AnimationTimeline::newKey()
		{
			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			if (set)
			{
				if (animationIndex >= 0 && animationIndex < set->animations.size() && selectedKey == -1)
					CommandManager::pushNew(new AddKeyCommand(set, animationIndex, currentFrame));
			}
		}

		void AnimationTimeline::deleteKey()
		{
			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			if (set && selectedKey != -1)
			{
				CommandManager::pushNew(new RemoveKeyCommand(set, animationIndex, selectedKey));
			}
		}

		void AnimationTimeline::copyKey()
		{
			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			if (!set)
				return;

			if (animationIndex < 0 || animationIndex >= set->animations.size())
				return;

			if (selectedKey >= 0 && selectedKey < set->animations[animationIndex].keys.size() - 1)
			{
				copyiedKey = set->animations[animationIndex].keys[selectedKey];
				canPaste = true;
			}
		}

		void AnimationTimeline::pasteKey()
		{
			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			if (!set)
				return;

			if (animationIndex < 0 || animationIndex >= set->animations.size())
				return;

			if (currentFrame >= set->animations[animationIndex].getStart() && currentFrame <= set->animations[animationIndex].getEnd()
				&& selectedKey == -1 && canPaste)
			{
				EditorKeyframe k = copyiedKey;
				k.time = currentFrame;
				CommandManager::pushNew(new AddKeyCommand(set, animationIndex, k));
			}
		}

		void AnimationTimeline::keyTooltip(const EditorKeyframe& k)
		{
			if (!showKeyTooltip)
				return;

			std::shared_ptr<EditorAnimationSet> set = animSet.lock();

			ImGui::BeginTooltip();
			ImGui::Text("Time: %d", k.time);
			ImGui::Text("Value: %.3f", k.value);

			if (set->type == AnimationSetType::Glitter)
			{
				ImGui::Text("InParam: %.3f", k.inParam);
				ImGui::Text("OutParam: %.3f", k.outParam);
				ImGui::Text("Range: %.3f", k.randomRange);
			}

			ImGui::EndTooltip();
		}

		void AnimationTimeline::updateCursor()
		{
			selectedKey = -1;

			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			if (!set || animationIndex >= set->animations.size() || animationIndex < 0)
				return;

			if (ImGui::GetIO().MouseClicked[0] && hovered && !holdingTan && !ImGui::IsPopupOpen((unsigned int)0, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
			{
				int frame = ((ImGui::GetIO().MousePos.x - canvasPos.x) / effectiveFrameWidth) + frameStart;
				currentFrame = frame;
				if (currentFrame < 0)
					currentFrame = 0;
			}

			const std::vector<EditorKeyframe>& keys = set->animations[animationIndex].keys;
			for (int i = 0; i < keys.size(); ++i)
			{
				if (currentFrame == keys[i].time)
					selectedKey = i;
			}

			// draw cursor
			ImVec2 cursorPos = ImVec2(canvasPos.x + ((currentFrame - frameStart) * effectiveFrameWidth), canvasPos.y);
			if (Utilities::isWithinRange(cursorPos.x, canvasPos.x + timelinePosOffset.x - 10, canvasPos.x + timelinePosOffset.x + canvasSize.x + 10))
			{
				ImU32 cursorColor = ImGui::GetColorU32(ImVec4(0.8, 0.8, 0.8, 0.75));
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				drawList->AddLine(cursorPos, ImVec2(cursorPos.x, canvasPos.y + canvasSize.y), cursorColor, 2.5f);
				drawList->AddTriangleFilled(ImVec2(cursorPos.x - 10, cursorPos.y), ImVec2(cursorPos.x, cursorPos.y + 10), ImVec2(cursorPos.x + 10, cursorPos.y), cursorColor);
			}
		}

		void AnimationTimeline::updateDopesheet()
		{
			
		}

		void AnimationTimeline::updateCurveEditor()
		{
			std::shared_ptr<EditorAnimationSet> set = animSet.lock();
			if (!set)
				return;

			if (animationIndex < 0 || animationIndex >= set->animations.size())
				return;

			std::vector<EditorKeyframe>& keys = set->animations[animationIndex].keys;
			for (int i = 0; i < keys.size(); ++i)
			{
				if (i < keys.size() - 1)
					drawAnimationCurve(keys[i], keys[i + 1]);

				// get key position in timeline
				float x{ canvasPos.x + ((keys[i].time - frameStart) * effectiveFrameWidth) };
				float y{ valueToHeight(keys[i].value) };

				static EditorKeyframe k;
				bool anyDeactivated = false;
				bool edit = false;

				// Only draw keys when they are within the canvas area
				if (Utilities::isWithinRange(x, canvasPos.x + timelinePosOffset.x - 10, canvasPos.x + timelinePosOffset.x + canvasSize.x + 10)
					&& Utilities::isWithinRange(y, canvasPos.y - 10, canvasPos.y + canvasSize.y))
				{
					const bool hermite = keys[i].interpolation == Glitter::InterpolationType::Hermite;
					const ImU32 redKey = ImGui::GetColorU32(ImVec4(1.0, 0.0, 0.0, 0.75));
					const ImU32 bluKey = ImGui::GetColorU32(ImVec4(0.0, 0.0, 1.0, 0.75));
					const ImU32 circleColor = hermite ? bluKey : redKey;

					ImGui::SetCursorScreenPos(ImVec2(x - (UI::btnSmall.x / 2.0f), y - (UI::btnSmall.y / 2.0f)));
					ImGui::InvisibleButton(std::string("##circle" + i).c_str(), UI::btnSmall);

					if (ImGui::IsItemActivated()) k = keys[i];
					if (ImGui::IsItemActive())
					{
						keyTooltip(keys[i]);
						if (ImGui::IsMouseDragging(0))
						{
							ImGuiIO& io = ImGui::GetIO();
							y += io.MouseDelta.y;

							selectedKey = i;
							currentFrame = ((io.MousePos.x - canvasPos.x) / effectiveFrameWidth) + frameStart;
							currentFrame = std::clamp(currentFrame, frameStart, frameEnd);
							keys[i].time = currentFrame;
							keys[i].time = set->animations[animationIndex].verifyKeyOrder(i, keys[i].time);
							keys[i].value = std::clamp(heightToValue(y), lowerLimits[limitIndex], higherLimits[limitIndex]);

							edit = true;
						}
					}

					anyDeactivated |= ImGui::IsItemDeactivated();

					ImDrawList* drawList = ImGui::GetWindowDrawList();
					drawList->AddCircleFilled(ImVec2(x, y), keyRadiusOut, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 12);
					drawList->AddCircleFilled(ImVec2(x, y), keyRadiusIn, circleColor, 12);

					// Tangent adjustment for GlitterAnimation keys
					if (set->type == AnimationSetType::Glitter)
					{
						ImVec2 circlePos = ImVec2(x - 50, y + keys[i].inParam * 10);
						drawList->AddLine(ImVec2(x, y), circlePos, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 3);

						if (Utilities::isWithinRange(circlePos.y, canvasPos.y, canvasPos.y + canvasSize.y + 10))
						{
							// left tangent
							ImGui::SetCursorScreenPos(ImVec2(circlePos.x, circlePos.y - (UI::btnSmall.y / 2.0f)));
							ImGui::InvisibleButton(std::string("tan_in" + std::to_string(i)).c_str(), UI::btnSmall);

							if (ImGui::IsItemActivated()) k = keys[i];
							if (ImGui::IsItemActive())
							{
								holdingTan = true;
								if (ImGui::IsMouseDragging(0))
								{
									onHandleDrag(keys[i], i, circlePos);
									keys[i].inParam = (circlePos.y - y) / 10.0f;

									edit = true;
								}
							}

							anyDeactivated |= ImGui::IsItemDeactivated();
							drawList->AddCircleFilled(circlePos, keyRadiusIn, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 12);
						}

						circlePos = ImVec2(x + 50, y - keys[i].outParam * 10);
						drawList->AddLine(ImVec2(x, y), circlePos, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 3);

						if (Utilities::isWithinRange(circlePos.y, canvasPos.y, canvasPos.y + canvasSize.y + 10))
						{
							// right tangent
							ImGui::SetCursorScreenPos(ImVec2(circlePos.x - (UI::btnSmall.x / 2.0f), circlePos.y - (UI::btnSmall.y / 2.0f)));
							ImGui::InvisibleButton(std::string("tan_out" + std::to_string(i)).c_str(), UI::btnSmall);

							if (ImGui::IsItemActivated()) k = keys[i];
							if (ImGui::IsItemActive())
							{
								holdingTan = true;
								if (ImGui::IsMouseDragging(0))
								{
									onHandleDrag(keys[i], i, circlePos);
									keys[i].outParam = -(circlePos.y - y) / 10.0f;

									edit = true;
								}
							}

							anyDeactivated |= ImGui::IsItemDeactivated();
							drawList->AddCircleFilled(circlePos, keyRadiusIn, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 12);
						}
					}

					if (edit && k != keys[i])
						set->markDirty(true);

					if (anyDeactivated && k != keys[i])
					{
						EditorKeyframe temp = k;
						k = keys[i];
						keys[i] = temp;
						CommandManager::pushNew(new ChangeKeyCommand(set, animationIndex, k, i));
					}

					drawList->AddCircleFilled(ImVec2(x, y), keyRadiusOut, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 12);
					drawList->AddCircleFilled(ImVec2(x, y), keyRadiusIn, circleColor, 12);
				}
			}
		}

		void AnimationTimeline::update()
		{
			if (ImGui::Begin(name))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 3));
				ImGui::Columns(3, "timeline_cols", false);
				ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x / 6.0f);
				ImGui::SetColumnWidth(1, ImGui::GetWindowSize().x * (3.75f / 6.0f));
				ImGui::SetColumnWidth(2, ImGui::GetWindowSize().x * (2.0f / 6.0f));
				updateAnimationControls();
				animationProperties();
				ImGui::NextColumn();

				frameStart = 0;
				frameEnd = 0;
				holdingTan = false;
				std::shared_ptr<EditorAnimationSet> set = animSet.lock();
				
				// workaround to set currentFrame to 0 if not valid animation or animation set is selected
				int saveFrame = currentFrame;
				currentFrame = 0;

				if (set)
				{
					if (animationIndex > -1 && animationIndex < set->animations.size())
					{
						EditorAnimation anim = set->animations[animationIndex];
						frameStart = anim.getStart();
						frameEnd = anim.getEnd();
						limitIndex = (int)getGraphType(set->animations[animationIndex].type);
						currentFrame = saveFrame;
					}
				}

				hovered = ImGui::IsMouseHoveringRect(boundaries.Min, boundaries.Max);
				if (hovered)
					timelinePosOffset.x -= ImGui::GetIO().MouseWheel * 25;

				//timelineMaxOffset = ImVec2(std::max(((frameEnd - frameStart) * effectiveFrameWidth) - ((floorf(canvasSize.x)) / 1.5f), 0.0f), 0.0f);
				if (timelinePosOffset.x < timelineMinOffset.x)
					timelinePosOffset.x = timelineMinOffset.x;

				drawTimelineBase();

				if (mode == TimelineMode::DopeSheet)
					updateDopesheet();
				else if (mode == TimelineMode::CurveEditor)
					updateCurveEditor();

				updateCursor();

				ImGui::EndChild();

				ImGui::NextColumn();
				keyProperties();
				ImGui::PopStyleVar();
			}

			ImGui::End();
		}

		AnimationTimeline::AnimationGraphType AnimationTimeline::getGraphType(Glitter::AnimationType type) const
		{
			size_t typeIndex = (size_t)type;
			if (Utilities::isWithinRange(typeIndex, 0, 2))
			{
				return AnimationGraphType::Translate;
			}
			else if (Utilities::isWithinRange(typeIndex, 3, 5))
			{
				return AnimationGraphType::Rotate;
			}
			else if (Utilities::isWithinRange(typeIndex, 6, 9))
			{
				return AnimationGraphType::Scale;
			}
			else if (Utilities::isWithinRange(typeIndex, 10, 13))
			{
				return AnimationGraphType::Color;
			}
			else if (Utilities::isWithinRange(typeIndex, 14, 21))
			{
				return AnimationGraphType::UV;
			}
			else
			{
				return AnimationGraphType::Integer;
			}
		}

		float AnimationTimeline::valueToHeight(float val)
		{
			const float percent = 1 - ((val - lowerLimits[limitIndex]) / (higherLimits[limitIndex] - lowerLimits[limitIndex]));
			return canvasPos.y + (percent * (canvasSize.y - heightOffset)) + heightOffset;
		}

		float AnimationTimeline::heightToValue(float height)
		{
			const float percent = (height - canvasPos.y - heightOffset) / (canvasSize.y - heightOffset);
			return ((1 - percent) * (higherLimits[limitIndex] - lowerLimits[limitIndex])) + lowerLimits[limitIndex];
		}

		/*
void AnimationTimeline::updateCurveScroll()
{
	const float scrollHeight = 15.0f;
	float winY = ImGui::GetCurrentWindow()->Pos.y + ImGui::GetCurrentWindow()->Size.y - scrollHeight - 5.0f;

	scrollbarPos = ImVec2(ImGui::GetCurrentWindow()->Pos.x, winY);
	scrollbarSize = ImGui::GetCurrentWindow()->Size;
	scrollBoundaries = ImRect(scrollbarPos, ImVec2(scrollbarPos.x + scrollbarSize.x, scrollbarPos.y + scrollHeight));

	ImGui::RenderFrame(scrollBoundaries.Min, scrollBoundaries.Max, ImGui::GetColorU32(ImGuiCol_ScrollbarBg));

	float maxPos = timelineMaxOffset.x + abs(timelineMinOffset.x);
	float curPos = timelinePosOffset.x + abs(timelineMinOffset.x);
	float ratio = curPos / maxPos;
	int visibleFrames = canvasSize.x / effectiveFrameWidth;
	int maxFrames = ((timelineMaxOffset.x + abs(timelineMinOffset.x) + canvasSize.x) / effectiveFrameWidth) + frameStart;

	float sizeRatio = ((float)visibleFrames / (float)maxFrames);
	sizeRatio = std::clamp(sizeRatio, 0.05f, 1.0f);
	float size = scrollbarSize.x * sizeRatio;

	// we subtract barOffset.x twice for barMax because it is already included in barMin.
	ImVec2 barOffset(5.0f, 2.5f);
	ImVec2 barMin = ImVec2(scrollBoundaries.Min.x + ratio * (scrollBoundaries.Max.x - scrollBoundaries.Min.x - size), scrollbarPos.y) + barOffset;
	ImVec2 barMax = ImVec2(barMin.x - barOffset.x + size, scrollbarPos.y + scrollHeight) - barOffset;

	ImGui::SetCursorScreenPos(barMin);
	ImGui::InvisibleButton("scroll_handle", barMax - barMin);
	if (ImGui::IsItemActive())
	{
		timelinePosOffset.x += ImGui::GetIO().MouseDelta.x * zoom;
		timelinePosOffset.x = std::clamp(timelinePosOffset.x, timelineMinOffset.x, timelineMaxOffset.x);
	}

	ImGui::GetWindowDrawList()->AddRectFilled(barMin, barMax, ImGui::GetColorU32(ImGuiCol_ScrollbarGrab), 15.0f);
}
*/
	}
}

#include "AnimationTimeline.h"
#include "UiHelper.h"
#include "Utilities.h"
#include "AnimationCommands.h"
#include "CommandManager.h"
#include "IconsFontAwesome5.h"
#include "Logger.h"
#include <algorithm>

AnimationTimeline::AnimationTimeline() :
	zoom{ 2.5f }, minZoom{ 1.0f }, maxZoom{ 5.0f }, frameWidth{ 8 }, currentFrame{ 0 }, holdingTan{ false }
{
	io = &ImGui::GetIO();

	frameStart = frameEnd = 0;
	timelineMinOffset = ImVec2(-100, 0);
	timelinePosOffset = ImVec2(timelineMinOffset.x, 0);

	animation = nullptr;
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

void AnimationTimeline::updateCurveEdit()
{
	drawList	= ImGui::GetWindowDrawList();
	canvasSize	= ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 10);
	canvasPos	= ImGui::GetCursorScreenPos();
	boundaries	= ImRect(canvasPos, canvasPos + canvasSize);
	
	ImGui::ItemSize(boundaries);
	hovered	= ImGui::IsMouseHoveringRect(boundaries.Min, boundaries.Max);
	effectiveFrameWidth = frameWidth * zoom;
	canvasPos -= timelinePosOffset;

	ImU32 bgColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
	ImGui::RenderFrame(boundaries.Min, boundaries.Max, bgColor, true, 1.0f);

	updateCurveScroll();

	if (animation)
	{
		frameStart = animation->getStartTime();
		frameEnd = animation->getEndTime();
	}
	else
	{
		frameStart = frameEnd = 0;
		currentFrame = 0;
		selectedKey = -1;
	}

	timelineMaxOffset = ImVec2(std::max(((frameEnd - frameStart) * effectiveFrameWidth) - ((floorf(canvasSize.x)) / 1.5f), 0.0f), 0.0f);

	holdingTan = false;
	drawList->PushClipRect(boundaries.Min, boundaries.Max, true);

	updateTimelineContents();
	updateTimelineKeys();
	updateMarker();

	if (hovered)
		timelinePosOffset.x -= io->MouseWheel * 25;

	timelinePosOffset.x = std::clamp(timelinePosOffset.x, timelineMinOffset.x, timelineMaxOffset.x);
	drawList->PopClipRect();
}

void AnimationTimeline::updateCurveScroll()
{
	const float scrollHeight = 15.0f;
	float winY = ImGui::GetCurrentWindow()->Pos.y + ImGui::GetCurrentWindow()->Size.y - scrollHeight -5.0f;
	
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

	drawList->AddRectFilled(barMin, barMax, ImGui::GetColorU32(ImGuiCol_ScrollbarGrab), 15.0f);
}

int AnimationTimeline::getCurrentFrame() const
{
	return currentFrame;
}

int AnimationTimeline::getSelectedKey() const
{
	return selectedKey;
}

void AnimationTimeline::setCurrentFrame(int frame)
{
	if (frame >= frameStart && frame <= frameEnd)
		currentFrame = frame;
}

void AnimationTimeline::updateControls()
{
	// Zoom //
	float zoomPercent = zoom * 100.0f;

	if (ImGui::Button(ICON_FA_SEARCH_MINUS, ImVec2(25, 25)))
		zoomPercent -= 50.0f;

	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 2.0f));
	ImGui::SetNextItemWidth(150.0f);
	ImGui::SliderFloat("##zoom_slider", &zoomPercent, minZoom * 100.0f, maxZoom * 100.0f, "%.1f%%");

	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_SEARCH_PLUS, ImVec2(25, 25)))
		zoomPercent += 50.0f;

	zoom = zoomPercent / 100.0f;

	// Jump To Key //
	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_FAST_BACKWARD, ImVec2(30, 25)))
		firstKey();

	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_BACKWARD, ImVec2(30, 25)))
		previousKey();

	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_FORWARD, ImVec2(30, 25)))
		nextKey();

	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_FAST_FORWARD, ImVec2(30, 25)))
		lastKey();

	currentFrame = std::clamp(currentFrame, frameStart, frameEnd);
	zoom = std::clamp(zoom, minZoom, maxZoom);

	// Key Creation/Deletion //
	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_PLUS_CIRCLE, ImVec2(30, 25)))
		newKey();

	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_MINUS_CIRCLE, ImVec2(30, 25)))
		deleteKey();

	// Limits //
	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	ImGui::Text("Lower/Higher Limit");
	ImGui::SameLine();
	static float lim[2]{ lowerLimits[limitIndex], higherLimits[limitIndex] };
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	if (ImGui::DragFloat2("##lim", lim, 1.0f, 0.0f, 0.0f, "%.2f"))
	{
		if (lim[0] < lim[1])
		{
			lowerLimits[limitIndex] = lim[0];
			higherLimits[limitIndex] = lim[1];
		}
	}

	ImGui::Separator();
	ImGui::PopStyleVar(1);
}

void AnimationTimeline::update(std::weak_ptr<AnimationNode> &node, size_t index)
{
	this->node = node;
	this->animation = &(node.lock()->getAnimationList()->at(index));
	animationIndex = index;
	limitIndex = (size_t)getGraphType(animation->getType());

	updateControls();
	updateCurveEdit();
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

void AnimationTimeline::updateMarker()
{
	if (animation)
	{
		if (io->MouseClicked[0] && hovered && !holdingTan)
		{
			currentFrame = ((io->MousePos.x - canvasPos.x) / effectiveFrameWidth) + frameStart;
		}

		currentFrame = std::clamp(currentFrame, frameStart, frameEnd);

		// set selected key if changed here to prevent animation properties window from scrolling to the top
		if (selectedKey > -1)
		{
			selectedKey = -1;
			for (int i = 0; i < animation->getKeys().size(); ++i)
			{
				if (currentFrame == animation->getKeys()[i].time)
				{
					selectedKey = i;
					break;
				}
			}
		}
	}

	// draw cursor
	ImVec2 cursorPos = ImVec2(canvasPos.x + ((currentFrame - frameStart) * effectiveFrameWidth), canvasPos.y);
	if (Utilities::isWithinRange(cursorPos.x, canvasPos.x + timelinePosOffset.x - 10, canvasPos.x + timelinePosOffset.x + canvasSize.x + 10))
	{
		ImU32 cursorColor = ImGui::GetColorU32(ImVec4(0.8, 0.8, 0.8, 0.75));
		drawList->AddLine(cursorPos, ImVec2(cursorPos.x, canvasPos.y + canvasSize.y), cursorColor, 2.5f);
		drawList->AddTriangleFilled(ImVec2(cursorPos.x - 10, cursorPos.y), ImVec2(cursorPos.x, cursorPos.y + 10), ImVec2(cursorPos.x + 10, cursorPos.y), cursorColor);
	}
}

void AnimationTimeline::updateTimelineContents()
{
	int start = std::max((timelinePosOffset.x / effectiveFrameWidth) + frameStart, 0.0f);
	int end = ((timelinePosOffset.x + canvasSize.x) / effectiveFrameWidth) + frameStart;

	for (int frame = start; frame <= end; ++frame)
	{
		const float x = canvasPos.x + ((frame - frameStart) * effectiveFrameWidth);
		const float y = canvasPos.y + ((frame & 1) ? 10 : 0) + canvasSize.y;

		const bool boldFrame = frame % 10 == 0 || frame == frameStart || frame == frameEnd;
		const bool validFrame = frame >= frameStart && frame <= frameEnd;
		
		ImU32 lineColor;
		if (validFrame)
			lineColor = boldFrame ? ImGui::GetColorU32(ImVec4(0.50, 0.50, 0.50, 0.75)) : ImGui::GetColorU32(ImVec4(0.40, 0.40, 0.40, 0.45));
		else
			lineColor = ImGui::GetColorU32(ImVec4(0.2, 0.2, 0.2, 0.75f));
		drawList->AddLine(ImVec2(x, y - canvasSize.y), ImVec2(x, y), lineColor, 0.80f);

		if (boldFrame)
			drawList->AddText(ImVec2(x + 3, canvasPos.y), lineColor, std::to_string(frame).c_str());
	}

	// draw horizontal lines
	if (animation)
	{
		int count = 0;
		while (count < 5)
		{
			const float xPos{ canvasPos.x + timelinePosOffset.x };
			const float yPos{ canvasPos.y + (0.25f * count * (canvasSize.y - 20.0f)) + 20.0f };

			ImU32 lineColor = (count & 1) ? ImGui::GetColorU32(ImVec4(0.35, 0.35, 0.35, 0.45)) : ImGui::GetColorU32(ImVec4(0.45, 0.45, 0.45, 0.75));
			bool last{ count == 4 };
			if (count && !last)
				drawList->AddLine(ImVec2(xPos, yPos), ImVec2(xPos + canvasSize.x, yPos), lineColor, 0.80f);
			
			float val = higherLimits[limitIndex] - (count * 0.25f) * (higherLimits[limitIndex] - lowerLimits[limitIndex]);
			size_t length = snprintf(NULL, 0, "%.2f", val);
			char* str = (char*)malloc(length + 1);
			snprintf(str, length + 1, "%.2f", val);
			
			drawList->AddText(ImVec2(xPos + 2, yPos + (last ? -20 : -2)), ImGui::GetColorU32(ImVec4(0.65, 0.65, 0.65, 0.85)), str);
			free(str);

			++count;
		}
	}

	// draw top horizontal line
	drawList->AddLine(ImVec2(canvasPos.x + timelinePosOffset.x, canvasPos.y + 20), ImVec2(canvasPos.x + canvasSize.x + timelinePosOffset.x, canvasPos.y + 20),
		ImGui::GetColorU32(ImVec4(0.5, 0.5, 0.5, 0.85)), 2.0f);
}

void AnimationTimeline::onHandleDrag(Glitter::Key& k, int index, ImVec2& pos)
{
	currentFrame = k.time;
	selectedKey = index;
	pos.y += io->MouseDelta.y;

	if (pos.y < canvasPos.y + 1.0f)
		pos.y = canvasPos.y + 1.0f;
	if (pos.y > canvasPos.y + canvasSize.y - 1.0f)
		pos.y = canvasPos.y + canvasSize.y - 1.0f;

	holdingTan = true;
}

void AnimationTimeline::updateTimelineKeys()
{
	if (!animation)
		return;

	std::vector<Glitter::Key>& keys{ animation->getKeys() };
	for (size_t i = 0; i < keys.size(); ++i)
	{
		if (i < keys.size() - 1)
			drawAnimationCurve(keys[i], keys[i + 1]);

		float x{ canvasPos.x + ((keys[i].time - frameStart) * effectiveFrameWidth) };
		float y{ valueToHeight(keys[i].value) };

		static Glitter::Key k;
		bool anyDeacvtivated = false;

		// Only draw keys when they are within the canvas area
		if (Utilities::isWithinRange(x, canvasPos.x + timelinePosOffset.x - 10, canvasPos.x + timelinePosOffset.x + canvasSize.x + 10)
			&& Utilities::isWithinRange(y, canvasPos.y - 10, canvasPos.y + canvasSize.y))
		{
			bool hermite = keys[i].interpolationType == Glitter::InterpolationType::Hermite;
			const ImU32 circleColor = hermite ? ImGui::GetColorU32(ImVec4(0.0, 0.0, 1.0, 0.75)) : ImGui::GetColorU32(ImVec4(1.0, 0.0, 0.0, 0.75));
			ImGui::SetCursorScreenPos(ImVec2(x - 10.0f, y - 10.0f));
			ImGui::InvisibleButton(std::string("##circle" + i).c_str(), ImVec2(20, 20));

			// capture the initial state of the key once any of the buttons (key, or tangent handles) are active
			// then push the initial and final state to the command's constructor
			if (ImGui::IsItemActivated())
				k = keys[i];

			if (ImGui::IsItemActive())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Time: %.0f", keys[i].time);
				ImGui::Text("Value: %.2f", keys[i].value);
				ImGui::EndTooltip();

				if (ImGui::IsMouseDragging(0))
				{
					y += io->MouseDelta.y;

					selectedKey = i;
					currentFrame = ((io->MousePos.x - canvasPos.x) / effectiveFrameWidth) + frameStart;
					currentFrame = std::clamp(currentFrame, frameStart, frameEnd);
					keys[i].time = currentFrame;
					keys[i].value = std::clamp(heightToValue(y), lowerLimits[limitIndex], higherLimits[limitIndex]);
					animation->verifyKeyOrder(i);
				}
			}

			anyDeacvtivated |= ImGui::IsItemDeactivated();

			ImVec2 circlePos = ImVec2(x - 50, y + keys[i].inParam * 10);
			drawList->AddLine(ImVec2(x, y), circlePos, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 3);

			if (Utilities::isWithinRange(circlePos.y, canvasPos.y, canvasPos.y + canvasSize.y + 10))
			{
				ImGui::SetCursorScreenPos(ImVec2(circlePos.x, circlePos.y - 10));
				ImGui::InvisibleButton(std::string("tan_in" + std::to_string(i)).c_str(), ImVec2(20, 20));

				if (ImGui::IsItemActivated())
					k = keys[i];

				// left tangent
				if (ImGui::IsItemActive())
				{
					ImGui::BeginTooltip();
					ImGui::Text("InParam: %.2f", keys[i].inParam);
					ImGui::Text("OutParam: %.2f", keys[i].outParam);
					ImGui::EndTooltip();

					if (ImGui::IsMouseDragging(0))
					{
						onHandleDrag(keys[i], i, circlePos);
						keys[i].inParam = (circlePos.y - y) / 10.0f;
					}
				}

				anyDeacvtivated |= ImGui::IsItemDeactivated();
				drawList->AddCircleFilled(circlePos, 6, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 12);
			}

			circlePos = ImVec2(x + 50, y - keys[i].outParam * 10);
			drawList->AddLine(ImVec2(x, y), circlePos, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 3);
			
			if (Utilities::isWithinRange(circlePos.y, canvasPos.y, canvasPos.y + canvasSize.y + 10))
			{
				ImGui::SetCursorScreenPos(ImVec2(circlePos.x - 10, circlePos.y - 10));
				ImGui::InvisibleButton(std::string("tan_out" + std::to_string(i)).c_str(), ImVec2(20, 20));

				if (ImGui::IsItemActivated())
					k = keys[i];

				// right tangent
				if (ImGui::IsItemActive())
				{
					ImGui::BeginTooltip();
					ImGui::Text("InParam: %.2f", keys[i].inParam);
					ImGui::Text("OutParam: %.2f", keys[i].outParam);
					ImGui::EndTooltip();

					if (ImGui::IsMouseDragging(0))
					{
						onHandleDrag(keys[i], i, circlePos);
						keys[i].outParam = -(circlePos.y - y) / 10.0f;
					}
				}

				anyDeacvtivated |= ImGui::IsItemDeactivated();
				drawList->AddCircleFilled(circlePos, 6, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 12);
			}

			if (anyDeacvtivated && k != keys[i])
			{
				auto ptr = node.lock();
				Glitter::Key temp = k;
				k = keys[i];
				keys[i] = temp;
				CommandManager::pushNew(new ChangeKeyCommand(ptr, animationIndex, i, k));
			}

			drawList->AddCircleFilled(ImVec2(x, y), 8.0f, ImU32(ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75))), 12);
			drawList->AddCircleFilled(ImVec2(x, y), 6.0f, circleColor, 12);
		}

		if (keys[i].time == currentFrame)
			selectedKey = i;
	}
}

void AnimationTimeline::newKey()
{
	if (selectedKey != -1 && animation->getKeys()[selectedKey].time == currentFrame)
		return;

	auto nodePtr = node.lock();
	CommandManager::pushNew(new AddKeyCommand(nodePtr, animationIndex, currentFrame));
}

void AnimationTimeline::deleteKey()
{
	if (selectedKey >= 0)
	{
		auto nodePtr = node.lock();
		CommandManager::pushNew(new RemoveKeyCommand(nodePtr, animationIndex, selectedKey));
		selectedKey = -1;
	}
}

void AnimationTimeline::previousKey()
{
	std::vector<Glitter::Key> keys = animation->getKeys();
	for (size_t index = 0; index < keys.size(); ++index)
	{
		if (keys[index].time >= currentFrame)
		{
			currentFrame = index > 0 ? keys[index - 1].time : keys[index].time;
			//timelinePosOffset.x = canvasPos.x + ((currentFrame - frameStart) * effectiveFrameWidth) + timelineMinOffset.x;
			return;
		}
	}
}

void AnimationTimeline::nextKey()
{
	std::vector<Glitter::Key> keys = animation->getKeys();
	for (size_t index = 0; index < keys.size(); ++index)
	{
		if (keys[index].time >= currentFrame)
		{
			currentFrame = index < keys.size() - 1 ? keys[index + 1].time : keys[index].time;
			//timelinePosOffset.x = canvasPos.x + ((currentFrame - frameStart) * effectiveFrameWidth);
			return;
		}
	}
}

void AnimationTimeline::firstKey()
{
	std::vector<Glitter::Key> keys = animation->getKeys();
	if (keys.size())
	{
		currentFrame = keys[0].time;
		//timelinePosOffset.x = canvasPos.x + ((currentFrame - frameStart) * effectiveFrameWidth);
	}
}

void AnimationTimeline::lastKey()
{
	std::vector<Glitter::Key> keys = animation->getKeys();
	if (keys.size())
	{
		currentFrame = keys[keys.size() - 1].time;
		//timelinePosOffset.x = canvasPos.x + ((currentFrame - frameStart) * effectiveFrameWidth);
	}
}

float AnimationTimeline::valueToHeight(float val)
{
	const float percent = 1 - ((val - lowerLimits[limitIndex]) / (higherLimits[limitIndex] - lowerLimits[limitIndex]));
	return canvasPos.y + (percent * (canvasSize.y - 20.0f)) + 20.0f;
}

float AnimationTimeline::heightToValue(float height)
{
	const float percent = (height - canvasPos.y - 20.0f) / (canvasSize.y - 20.0f);
	return ((1 - percent) * (higherLimits[limitIndex] - lowerLimits[limitIndex])) + lowerLimits[limitIndex];
}

void AnimationTimeline::drawAnimationCurve(const Glitter::Key &k1, const Glitter::Key &k2)
{
	const float x1{ canvasPos.x + ((k1.time - frameStart) * effectiveFrameWidth) };
	const float x2{ canvasPos.x + ((k2.time - frameStart) * effectiveFrameWidth) };

	// drawing curves can be expensive so return early if the two points are out of range
	if (((x1 < canvasPos.x + timelinePosOffset.x) && (x2 < canvasPos.x + timelinePosOffset.x)) ||
		(x1 > canvasPos.x + timelinePosOffset.x + canvasSize.x) && (x2 > canvasPos.x + timelinePosOffset.x + canvasSize.x))
		return;

	static const ImU32 lineColor{ ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 0.75)) };

	if (k1.interpolationType == Glitter::InterpolationType::Hermite)
	{
		// Hack but it works. Basically calculate the value at each frame and connect the 'points' to form a curve
		ImVec2* points = (ImVec2*)(malloc(sizeof ImVec2 * ((size_t)k2.time - (size_t)k1.time + 1)));
		
		for (size_t i = k1.time; i <= k2.time; ++i)
			points[i - (size_t)k1.time] = ImVec2(canvasPos.x + ((i - frameStart) * effectiveFrameWidth), valueToHeight(animation->interpolate(i, k1, k2)));

		drawList->AddPolyline(points, k2.time - k1.time + 1, lineColor, false, 3.0f);
		free(points);
	}
	else
	{
		const float y1 = valueToHeight(k1.value);
		const float y2 = valueToHeight(k2.value);

		if (k1.interpolationType == Glitter::InterpolationType::Linear)
		{
			drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), lineColor, 3.0f);
		}
		else
		{
			drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y1), lineColor, 3.0f);
			drawList->AddLine(ImVec2(x2, y1), ImVec2(x2, y2), lineColor, 3.0f);
		}
	}
}

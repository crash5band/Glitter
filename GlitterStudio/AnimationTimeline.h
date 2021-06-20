#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "EditorAnimationSet.h"
#include <memory>

namespace Glitter
{
	namespace Editor
	{
		class AnimationTimeline
		{
		public:
			enum class AnimationGraphType
			{
				Translate,
				Rotate,
				Scale,
				UV,
				Color,
				Integer,
				Max
			};

			enum class TimelineMode
			{
				DopeSheet,
				CurveEditor
			};

		private:
			const char* name;
			const int frameWidth;
			int frameStart;
			int frameEnd;
			float zoom;
			const float minZoom;
			const float maxZoom;
			const float keyRadiusIn;
			const float keyRadiusOut;
			const float heightOffset;
			int currentFrame;
			int selectedKey;
			bool hovered;
			bool holdingTan;
			bool showKeyTooltip;
			bool canPaste;
			std::weak_ptr<EditorAnimationSet> animSet;
			EditorKeyframe copyiedKey;
			int animationIndex;
			size_t limitIndex;
			std::array<float, 6> lowerLimits;
			std::array<float, 6> higherLimits;

			ImVec2 timelinePosOffset;
			ImVec2 timelineMinOffset;
			ImVec2 timelineMaxOffset;
			ImRect boundaries;
			ImRect scrollBoundaries;

			ImVec2 canvasPos;
			ImVec2 canvasSize;
			ImVec2 scrollbarPos;
			ImVec2 scrollbarSize;
			float effectiveFrameWidth;
			TimelineMode mode;

			void initLimits();
			float valueToHeight(float value);
			float heightToValue(float height);

			void timelineContextMenu();
			void timelineSettings();
			void updateTimleineControls();
			void updateAnimationControls();
			void onHandleDrag(EditorKeyframe& k, int index, ImVec2 &pos);
			void keyTooltip(const EditorKeyframe& k);
			void keyProperties();
			void drawAnimationCurve(const EditorKeyframe& k1, const EditorKeyframe& k2);
			void drawTimelineBase();
			void updateCursor();
			void updateDopesheet();
			void updateCurveEditor();
			void animationProperties();

		public:
			AnimationTimeline(const char* name);

			std::vector<std::string> getAvailableAnimations() const;
			AnimationGraphType getGraphType(Glitter::AnimationType type) const;
			int getCurrentFrame() const;
			int getSelectedKey() const;
			void newKey();
			void deleteKey();
			void copyKey();
			void pasteKey();
			void setAnimations(std::shared_ptr<EditorAnimationSet> set);
			void update();
		};
	}
}

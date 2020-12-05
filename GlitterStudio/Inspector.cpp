#include "Inspector.h"
#include "Editor.h"
#include "Utilities.h"
#include "UiHelper.h"
#include "Logger.h"
#include "AnimationCommands.h"
#include "Stopwatch.h"

Inspector::Inspector() : selectedAnimation{ -1 }
{
	node.reset();
	animationNode.reset();
}

void Inspector::setNode(std::shared_ptr<IGlitterNode> node)
{
	this->node = node;
	animationNode = node ? node->getAnimationNode() : std::weak_ptr<AnimationNode>();
}

std::vector<std::string_view> Inspector::getAvailableAnimations()
{
	std::vector<std::string_view> newList;
	for (size_t index = 0; index < Glitter::animationTypeTableSize; ++index)
		newList.push_back(Glitter::animationTypeTable[index]);

	// remove existing animations
	std::vector<Glitter::Animation> &anims = *animationNode.lock()->getAnimationList();
	for (std::vector<std::string_view>::iterator it = newList.begin(); it != newList.end();)
	{
		for (int count = 0; count < anims.size(); ++count)
		{
			if (Glitter::glitterEnumToString(Glitter::animationTypeTable, Glitter::animationTypeTableSize,
				(size_t)anims[count].getType()) == *it)
			{
				it = newList.erase(it);
			}
		}

		++it;
	}

	return newList;
}

void Inspector::inspectAnimation(int index)
{
	if (ImGui::TreeNodeEx("Animation Properties", ImGuiTreeNodeFlags_DefaultOpen))
	{	
		auto animNode = animationNode.lock();

		if (index >= 0 && index < animNode->getAnimationList()->size())
		{
			static const char* animProperties[]{ "Start", "End", "Repeat Type", "Flags" };

			Glitter::Animation& animation = animNode->getAnimationList()->at(index);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x / 3.0f);

			for (int i = 0; i < 4; ++i)
			{
				ImGui::AlignTextToFramePadding();
				ImGui::Text(animProperties[i]);
			}

			ImGui::NextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

			float start		= animation.getStartTime();
			float end		= animation.getEndTime();
			int repeatType	= (int)animation.getRepeatType();
			int flags		= animation.getRandomFlags();

			if (ImGui::InputFloat("##Start", &start, 1, 5, 0, ImGuiInputTextFlags_EnterReturnsTrue))
				animation.setStartTime(start);

			if (ImGui::InputFloat("##End", &end, 1, 5, 0, ImGuiInputTextFlags_EnterReturnsTrue))
				animation.setStartTime(end);

			std::string preview = Glitter::glitterEnumToString(Glitter::repeatTypeTable, Glitter::repeatTypeTableSize, repeatType);
			if (ImGui::BeginCombo("##repeat", preview.c_str()))
			{
				for (int n = 0; n < Glitter::repeatTypeTableSize; ++n)
				{
					const bool selected = repeatType == n;
					if (ImGui::Selectable(Glitter::repeatTypeTable[n].c_str(), selected))
					{
						repeatType = n;
						animation.setRepeatType((Glitter::RepeatType)repeatType);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (ImGui::InputInt("##falgs", &flags, 1, 2, ImGuiInputTextFlags_EnterReturnsTrue))
				animation.setRandomFlags(flags);

			ImGui::PopItemWidth();
			ImGui::NextColumn();
			ImGui::Columns(1);
			
#ifdef _DEBUG
			float val = animationNode.lock()->tryGetValue(animation.getType(), timeline.getCurrentFrame());
			ImGui::Text("Current value: %.2f", val);
#endif // DEBUG

		}
		ImGui::TreePop();
	}
}

void Inspector::inspectKey(int index)
{
	auto animNode = animationNode.lock();

	if (index < 0 || index >= animNode->getAnimationList()->size())
		return;

	Glitter::Animation& animation = (*animNode->getAnimationList())[index];

	int keyIndex = timeline.getSelectedKey();
	if (ImGui::TreeNodeEx("Key Properties", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (keyIndex >= 0 && keyIndex < animation.getKeys().size())
		{
			std::vector<Glitter::Key> &keys = animation.getKeys();
			Glitter::Key key = keys[keyIndex];

			static const char* keyProperties[]{ "Time", "Value", "Interpolation Type", "Tangent", "Random Range" };

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x / 3.0f);

			for (int i = 0; i < 5; ++i)
			{
				ImGui::AlignTextToFramePadding();
				ImGui::Text(keyProperties[i]);
			}

			ImGui::NextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

			if (ImGui::InputFloat("##Time", &key.time, 1, 10, 0, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				key.time = std::clamp((int)key.time, (int)animation.getStartTime(), (int)animation.getEndTime());
				animation.verifyKeyOrder(keyIndex);
				timeline.setCurrentFrame(key.time);
			}

			if (ImGui::InputFloat("##Value", &key.value, 1, 10, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (timeline.getGraphType(animation.getType()) == AnimationTimeline::AnimationGraphType::Color)
				{
					key.value = std::clamp(key.value, 0.0f, 255.0f);
				}
				else if (timeline.getGraphType(animation.getType()) == AnimationTimeline::AnimationGraphType::Integer && key.value < 0.0f)
				{
					key.value = 0.0f;
				}
			}

			addComboBoxProperty("##Interpolation_Type", Glitter::interpolationTypeTable, Glitter::interpolationTypeTableSize, key.interpolationType);

			float tan[]{ key.inParam, key.outParam };
			if (ImGui::InputFloat2("##Tangent", tan, 3, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				key.inParam = tan[0];
				key.outParam = tan[1];
			}

			ImGui::InputFloat("##Random_Range", &key.randomRange, 1, 10, 2, ImGuiInputTextFlags_EnterReturnsTrue);

			if (key != keys[keyIndex])
				CommandManager::pushNew(new ChangeKeyCommand(animNode, selectedAnimation, keyIndex, key));

			ImGui::PopItemWidth();
			ImGui::NextColumn();
			ImGui::Columns(1);
		}
		ImGui::TreePop();
	}
}

void Inspector::update()
{
	if (ImGui::Begin(propertiesWindow, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		if (node.lock())
		{
			Stopwatch inspectorTime("Inspector");
			node.lock()->populateInspector();
		}
		else
		{
			ImGui::TextWrapped("Select a node from the tree view to view its properties");
		}
	}
	ImGui::End();

	if (ImGui::Begin(animProperties, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		if (animationNode.lock())
		{
			auto animNode = animationNode.lock();

#ifdef _DEBUG
			if (ImGui::Button("Rebuild Animation Cache", ImVec2(ImGui::GetContentRegionAvail().x, 30)))
			{
				animNode->buildCache();
				Logger::log(Message(MessageType::Normal, "Rebuilt animation cache."));
			}
#endif

			std::vector<Glitter::Animation>& list = (*animNode->getAnimationList());
			addListProperty("##Animations", list, selectedAnimation);
			if (ImGui::Button("Add Animation", ImVec2(ImGui::GetContentRegionAvail().x / 2.0f, 30)))
			{
				available = getAvailableAnimations();
				ImGui::OpenPopup("Add Animation");
			}

			if (ImGui::BeginPopup("Add Animation"))
			{
				ImGui::Text("New Animation");
				ImGui::Separator();

				for (size_t n = 0; n < available.size(); ++n)
				{
					if (ImGui::Selectable(available[n].data()))
					{
						Glitter::AnimationType type = (Glitter::AnimationType)Glitter::glitterStringToEnum(Glitter::animationTypeTable,
							Glitter::animationTypeTableSize, available[n].data());
						CommandManager::pushNew(new AddAnimationCommand(animNode, Glitter::Animation(type, 0), animNode->getAnimationList()->size()));
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Remove Animation", ImVec2(ImGui::GetContentRegionAvail().x, 30)))
			{
				CommandManager::pushNew(new RemoveAnimationCommand(animNode, selectedAnimation));
			}

			ImGui::Separator();
			inspectAnimation(selectedAnimation);
			inspectKey(selectedAnimation);
		}
		else
		{
			ImGui::TextWrapped("Select an effect, emitter, or particle from the tree view to view its animations");
		}
	}
	ImGui::End();

	if (ImGui::Begin(curveEditor, NULL, ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		if (animationNode.lock() && selectedAnimation >= 0 && selectedAnimation < animationNode.lock()->getAnimationList()->size())
		{
			Stopwatch timelineTimer("Curve Editor");
			timeline.update(animationNode, selectedAnimation);
		}
		else
		{
			ImGui::TextWrapped("Select an animation to edit in the timeline.");
		}
	}
	ImGui::End();
}

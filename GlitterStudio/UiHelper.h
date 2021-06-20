#pragma once
#include "MathGens.h"
#include "GlitterAnimation.h"
#include "CommandManager.h"
#include "PropertyCommands.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "UI.h"
#include <stack>

using namespace Glitter::Editor;

static void beginPropertyColumn(const char* label)
{
	ImGui::Columns(2, 0, false);
	ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x / 3.0f);
	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
}

static void endPropertyColumn()
{
	ImGui::Columns(1);
}

template <typename T>
static void addTextProperty(const char* label, std::string val, std::shared_ptr<T> &obj, std::_Mem_fn<void (T::*)(std::string)> func)
{
	static std::string old;

	beginPropertyColumn(label);
	if (ImGui::InputText(std::string("##").append(label).c_str(), &val))
		func(obj.get(), val);

	ImGui::NextColumn();
	if (ImGui::IsItemActivated())
		old = val;

	if (ImGui::IsItemDeactivated() && old != val)
		CommandManager::pushNew(new ChangePropertyCommand(label, obj, old, val, func));
}

template <typename T>
static void addIntProperty(const char* label, int val, std::shared_ptr<T>& obj, std::_Mem_fn<void (T::*)(int)> func)
{
	static int old;

	beginPropertyColumn(label);
	if (ImGui::InputInt(std::string("##").append(label).c_str(), &val, 1, 10))
		func(obj.get(), val);

	ImGui::NextColumn();
	if (ImGui::IsItemActivated())
		old = val;

	if (ImGui::IsItemDeactivated() && old != val)
		CommandManager::pushNew(new ChangePropertyCommand(label, obj, old, val, func));
}

template <typename T>
static void addUIntProperty(const char* label, unsigned int val, std::shared_ptr<T>& obj, std::_Mem_fn<void (T::*)(unsigned int)> func)
{
	static unsigned int old;
	int num = val;

	beginPropertyColumn(label);
	if (ImGui::InputInt(std::string("##").append(label).c_str(), &num, 1, 10))
	{
		if (num < 0)
			num = 0;
		func(obj.get(), num);
	}

	ImGui::NextColumn();
	if (ImGui::IsItemActivated())
		old = val;

	if (ImGui::IsItemDeactivated() && old != num)
		CommandManager::pushNew(new ChangePropertyCommand(label, obj, old, (unsigned int)num, func));
}

template <typename T>
static void addFloatProperty(const char* label, float val, std::shared_ptr<T>& obj, std::_Mem_fn<void (T::*)(float)> func)
{
	static float old;

	beginPropertyColumn(label);
	if (ImGui::InputFloat(std::string("##").append(label).c_str(), &val, 1.0f, 5.0f, "%g"))
		func(obj.get(), val);

	ImGui::NextColumn();
	if (ImGui::IsItemActivated())
		old = val;

	if (ImGui::IsItemDeactivated() && old != val)
		CommandManager::pushNew(new ChangePropertyCommand(label, obj, old, val, func));
}

template <typename T>
static void addVector2Property(const char* label, Glitter::Vector2 val, std::shared_ptr<T>& obj, std::_Mem_fn<void (T::*)(Glitter::Vector2)> func)
{
	float v2[2] = { val.x, val.y };
	static Glitter::Vector2 old;

	beginPropertyColumn(label);
	if (ImGui::DragFloat2(std::string("##").append(label).c_str(), v2, 0.1f, 0.0f, 0.0f, "%g"))
		func(obj.get(), Glitter::Vector2(v2[0], v2[1]));

	ImGui::NextColumn();
	if (ImGui::IsItemActivated())
		old = val;

	if (ImGui::IsItemDeactivated() && old != val)
		CommandManager::pushNew(new ChangePropertyCommand(label, obj, old, Glitter::Vector2(v2[0], v2[1]), func));
}

template <typename T>
static void addVector3Property(const char* label, Glitter::Vector3 val, std::shared_ptr<T>& obj, std::_Mem_fn<void (T::*)(Glitter::Vector3)> func)
{
	float v3[3] = { val.x, val.y, val.z };
	static Glitter::Vector3 old;

	beginPropertyColumn(label);
	if (ImGui::DragFloat3(std::string("##").append(label).c_str(), v3, 0.1f, 0.0f, 0.0f, "%g"))
		func(obj.get(), Glitter::Vector3(v3[0], v3[1], v3[2]));

	ImGui::NextColumn();
	if (ImGui::IsItemActivated())
		old = val;

	if (ImGui::IsItemDeactivated() && old != val)
		CommandManager::pushNew(new ChangePropertyCommand(label, obj, old, Glitter::Vector3(v3[0], v3[1], v3[2]), func));
}

template <typename T>
static void addColorProperty(const char* label, Glitter::Color val, std::shared_ptr<T>& obj, std::_Mem_fn<void (T::*)(Glitter::Color)> func)
{
	float v4[4] = { val.r, val.g, val.b, val.a };
	static Glitter::Color old;

	beginPropertyColumn(label);
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB;
	flags |= ImGuiColorEditFlags_PickerHueWheel;

	if (ImGui::ColorEdit4(std::string("##").append(label).c_str(), v4, flags))
		func(obj.get(), Glitter::Color(v4[0], v4[1], v4[2], v4[3]));

	ImGui::NextColumn();
	if (ImGui::IsItemActivated())
		old = val;

	if (ImGui::IsItemDeactivated() && old != val)
		CommandManager::pushNew(new ChangePropertyCommand(label, obj, old, Glitter::Color(v4[0], v4[1], v4[2], v4[3]), func));
}

template <typename T, typename S>
static void addComboBoxProperty(const char* label, const std::string* items, const size_t count, S val, std::shared_ptr<T>& obj, std::_Mem_fn<void (T::*)(S)> func)
{
	int selectedIndex = size_t(val);
	std::string comboLabel = Glitter::glitterEnumToString(items, count, (size_t)val);

	beginPropertyColumn(label);
	if (ImGui::BeginCombo(std::string("##").append(label).c_str(), comboLabel.c_str()))
	{
		for (int n = 0; n < count; ++n)
		{
			const bool selected = selectedIndex == n;
			if (ImGui::Selectable(items[n].c_str(), selected))
			{
				selectedIndex = n;
				if ((S)(selectedIndex) != val)
					CommandManager::pushNew(new ChangePropertyCommand(label, obj, val, S(selectedIndex), func));
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	ImGui::NextColumn();
}

template <typename T>
static void addComboBoxProperty(const char* label, const std::string* items, const size_t count, T& val)
{
	static int selectedIndex = (size_t)val;
	std::string comboLabel = Glitter::glitterEnumToString(items, count, (size_t)val);
	if (ImGui::BeginCombo(label, comboLabel.c_str()))
	{
		for (int n = 0; n < count; ++n)
		{
			const bool selected = selectedIndex == n;
			if (ImGui::Selectable(items[n].c_str(), selected))
			{
				selectedIndex = n;
				val = (T)selectedIndex;
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

static void addListProperty(const char* label, const std::vector<Glitter::GlitterAnimation>& items, int& selectedIndex)
{
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	const size_t itemCount = items.size();
	if (ImGui::ListBoxHeader(label, itemCount, 3))
	{
		for (int n = 0; n < itemCount; ++n)
		{
			bool selected = selectedIndex == n;
			std::string name = Glitter::glitterEnumToString(Glitter::animationTypeTable, Glitter::animationTypeTableSize, (size_t)items[n].getType());
			if (ImGui::Selectable(name.c_str(), selected))
				selectedIndex = n;
		}

		ImGui::ListBoxFooter();
	}
}

static void addStackView(const char* label, std::stack<ICommand*>& items, float xRatio)
{
	const size_t itemCount = items.size();
	if (ImGui::ListBoxHeader(label, ImVec2(ImGui::GetContentRegionAvail().x * xRatio, ImGui::GetContentRegionAvail().y - UI::btnHeight - 10.0f)))
	{
		for (int n = 0; n < itemCount; ++n)
		{
			const char* desc = items.top()->getDescription();
			ImGui::Selectable(desc);

			items.pop();
		}

		ImGui::ListBoxFooter();
	}
}

template <typename T>
static void addFlagsProperty(const char* label, unsigned int flags, unsigned int flagVal, std::shared_ptr<T>& obj, std::_Mem_fn<void (T::*)(unsigned int)> func)
{
	unsigned int f = flags;
	if (ImGui::CheckboxFlags(label, &f, flagVal))
		CommandManager::pushNew(new ChangePropertyCommand(label, obj, flags, f, func));
}

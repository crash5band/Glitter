#include "Viewport.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "IconsFontAwesome5.h"
#include "UiHelper.h"

Viewport::Viewport() : camera{ Camera(CameraMode::Orbit) }, fBuffer{ RenderTarget(1920, 1080) }, light{ Light(LightType::Direct) }
{
	mouseInViewArea = false;
	lightEnabled = true;
}

void Viewport::use()
{
	viewArea = ImRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail());
	mouseInViewArea = ImGui::IsMouseHoveringRect(viewArea.Min, viewArea.Max, false);

	if (mouseInViewArea && ImGui::IsWindowFocused())
	{
		ImGuiIO& io = ImGui::GetIO();
		camera.update(io.MouseDown[0], io.MouseDown[1], io.MouseDelta.x, io.MouseDelta.y, io.MouseWheel);
	}

	size = Glitter::Vector2(viewArea.Max.x - viewArea.Min.x, viewArea.Max.y - viewArea.Min.y);

	fBuffer.use();
	fBuffer.clear();
}

void Viewport::end()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	auto texture = fBuffer.getTexture();
	ImGui::Image((void*)texture, ImGui::GetContentRegionAvail(), ImVec2(1, 1), ImVec2(0, 0),
		ImVec4(1, 1, 1, 1), ImVec4(0.1, 0.1, 0.1, 0.85));
}

void Viewport::cameraControl()
{
	ImGui::Button(ICON_FA_VIDEO, ImVec2(btnHeight, btnHeight));
	ImGui::SameLine();

	if (ImGui::BeginPopupContextItem("cam_popup", ImGuiMouseButton_Left))
	{
		static const char* modeStr[] {"Normal", "Orbit"};
		int mode = (int)camera.getMode();
		if (ImGui::BeginCombo("Mode", modeStr[mode]))
		{
			for (int i = 0; i < 2; ++i)
			{
				const bool selected = mode == i;
				if (ImGui::Selectable(modeStr[i], selected))
				{
					mode = i;
					camera.setMode((CameraMode)mode);
				}


				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		float y = camera.getYaw();
		float p = camera.getPitch();
		float fov = camera.getFOV();

		ImGui::Separator();
		ImGui::DragFloat("Yaw", &y, 1.0f, 0.0f, 0.0f, "%g");
		ImGui::DragFloat("Pitch", &p, 1.0f, -89.0f, 89.0f, "%g");
		
		if (ImGui::DragFloat("FOV", &fov, 1.0f, 10.0f, 120.0f, "%g"))
			camera.setFOV(fov);

		// orbit camera
		if (mode == 1)
		{
			ImGui::Separator();

			float r = camera.getDistance();
			if (ImGui::DragFloat("Distance", &r, 1.0f, 0.0f, 0.0f, "%g"))
				camera.setDistance(r);

			DirectX::XMVECTOR tgt = camera.getTarget();
			float t[3]{ tgt.m128_f32[0], tgt.m128_f32[1], tgt.m128_f32[2] };
			if (ImGui::DragFloat3("Target", t, 1.0f, 0.0, 0.0f, "%g"))
			{
				tgt = DirectX::XMVECTOR{ t[0], t[1], t[2], 1.0f };
				camera.setTarget(tgt);
			}
		}

		ImGui::Separator();
		ImGui::Text("Presets");

		if (ImGui::Button(ICON_FA_ANGLE_UP, ImVec2(btnHeight, btnHeight)))
		{
			y = -90;
			p = 15;
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ANGLE_RIGHT, ImVec2(btnHeight, btnHeight)))
		{
			y = 0;
			p = 15;
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ANGLE_LEFT, ImVec2(btnHeight, btnHeight)))
		{
			y = -180;
			p = 15;
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_ANGLE_DOWN, ImVec2(btnHeight, btnHeight)))
		{
			y = 90;
			p = 15;
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_LOCATION_ARROW, ImVec2(btnHeight, btnHeight)))
		{
			y = -45;
			p = 20;
		}

		if (y != camera.getYaw() || p != camera.getPitch())
			camera.setAngle(y, p);

		ImGui::Separator();
		if (ImGui::Button("Reset", ImVec2(ImGui::GetContentRegionAvail().x, btnHeight)))
			camera.reset();

		ImGui::EndPopup();
	}
}

void Viewport::setLight(Light l)
{
	light = l;
}

void Viewport::resetCamera()
{
	camera.reset();
}

void Viewport::setLightEnabled(bool v)
{
	lightEnabled = v;
}

Glitter::Vector2 Viewport::getSize() const
{
	return size;
}

Camera Viewport::getCamera() const
{
	return camera;
}

Light Viewport::getLight() const
{
	return light;
}

bool Viewport::isLightEnabled() const
{
	return lightEnabled;
}
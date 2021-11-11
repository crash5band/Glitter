#include "Viewport.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "IconsFontAwesome5.h"
#include "UiHelper.h"
#include "ImGui/imgui_internal.h"
#include "stb_image.h"

namespace Glitter
{
	namespace Editor
	{
		Viewport::Viewport() : mouseInViewArea{ false }, lightEnabled{ true }, drawMode{ 2 },
			camera{ Camera(CameraMode::Orbit) }, fBuffer{ Engine::RenderTarget(1920, 1080, 4) }
		{

		}

		Viewport::~Viewport()
		{
			fBuffer.dispose();
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

			fBuffer.resize(size.x * 1.5f, size.y * 1.5f);
			fBuffer.use();
			fBuffer.clear();
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT + drawMode);
		}

		void Viewport::end()
		{
			fBuffer.end();
			auto texture = fBuffer.getTexture();

			// The renderered texture should start from (0, 0) to (1, 1).
			// However, we reverse the v coordinates to flip the image so that it is upright.
			ImGui::Image((void*)texture, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0),
				ImVec4(1, 1, 1, 1), ImVec4(0.2, 0.2, 0.2, 0.85));

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		void Viewport::cameraControl()
		{
			UI::transparentButton(ICON_FA_VIDEO, UI::UI::btnNormal);
			if (ImGui::BeginPopupContextItem("cam_popup", ImGuiMouseButton_Left))
			{
				float y = camera.getYaw();
				float p = camera.getPitch();
				float fov = camera.getFOV();

				float height = ImGui::GetContentRegionAvail().x;
				ImGui::SliderFloat("Yaw", &y, -180, 90, "%g", ImGuiSliderFlags_ClampOnInput);
				ImGui::SliderFloat("Pitch", &p, -89, 89, "%g", ImGuiSliderFlags_ClampOnInput);

				if (ImGui::DragFloat("FOV", &fov, 1.0f, 10.0f, 120.0f, "%g"))
					camera.setFOV(fov);

				ImGui::Separator();

				float r = camera.getDistance();
				if (ImGui::DragFloat("Distance", &r, 1.0f, 0.0f, 0.0f, "%g"))
					camera.setDistance(r);

				DirectX::XMVECTOR tgt = camera.getTarget();
				float t[3]{ tgt.m128_f32[0], tgt.m128_f32[1], tgt.m128_f32[2] };

				ImGui::Separator();
				ImGui::Text("Presets");

				if (ImGui::Button(ICON_FA_ANGLE_UP, UI::btnNormal))
				{
					y = -90;
					p = 15;
				}

				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_ANGLE_RIGHT, UI::btnNormal))
				{
					y = 0;
					p = 15;
				}

				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_ANGLE_LEFT, UI::btnNormal))
				{
					y = -180;
					p = 15;
				}

				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_ANGLE_DOWN, UI::btnNormal))
				{
					y = 90;
					p = 15;
				}

				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_LOCATION_ARROW, UI::btnNormal))
				{
					y = -45;
					p = 20;
				}

				if (y != camera.getYaw() || p != camera.getPitch())
					camera.setAngle(y, p);

				ImGui::Separator();
				if (ImGui::Button("Reset", ImVec2(ImGui::GetContentRegionAvail().x, UI::btnHeight)))
					camera.reset();

				ImGui::EndPopup();
			}
		}

		void Viewport::lightControl()
		{
			UI::transparentButton(ICON_FA_LIGHTBULB, UI::btnNormal);
			if (ImGui::BeginPopupContextItem("light_popup", ImGuiMouseButton_Left))
			{
				ImGui::Checkbox("Enabled", &lightEnabled);
				ImGui::Separator();

				float p3[3]{ light.position.x, light.position.y, light.position.z };
				if (ImGui::DragFloat3("Position", p3, 0.1f, 0.0f, 0.0f, "%g"))
					light.position = Glitter::Vector3(p3[0], p3[1], p3[2]);

				float c3[3]{ light.color.r, light.color.g, light.color.b };
				if (ImGui::ColorEdit3("Color", c3))
					light.color = Glitter::Color(c3[0], c3[1], c3[2]);

				ImGui::Separator();
				ImGui::DragFloat("Ambient", &light.ambient, 0.05f, 0.0f, 1.0f, "%g");
				ImGui::DragFloat("Specular", &light.specular, 0.05f, 0.0f, 1.0f, "%g");

				ImGui::EndPopup();
			}
		}
		
		void Viewport::renderingControl()
		{
			UI::transparentButton(ICON_FA_IMAGE, UI::btnNormal);
			if (ImGui::BeginPopupContextItem("rendering_popup", ImGuiMouseButton_Left))
			{
				static const char* modes[]{ "Point", "Wireframe", "Polygon" };
				if (ImGui::BeginCombo("Rendering Mode", modes[drawMode]))
				{
					for (int i = 0; i < 3; ++i)
					{
						const bool selected = drawMode == i;
						if (ImGui::Selectable(modes[i], selected))
						{
							drawMode = i;
						}

						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				static int msaa[]{ 1, 2, 4, 8, 16 };
				static int selection = 2;
				if (ImGui::BeginCombo("MSAA", std::to_string(msaa[selection]).c_str()))
				{
					for (int i = 0; i < 5; ++i)
					{
						const bool selected = selection == i;
						if (ImGui::Selectable(std::to_string(msaa[i]).c_str(), selected))
						{
							selection = i;
							fBuffer.setSampleCount(msaa[selection]);
						}

						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				Color cc = fBuffer.getClearColor();
				float col[4]{ cc.r, cc.g, cc.b, cc.a };
				if (ImGui::ColorEdit4("Clear Color", col, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB))
				{
					Color result(col[0], col[1], col[2], col[3]);
					fBuffer.setClearColor(result);
				}

				ImGui::EndPopup();
			}
		}

		void Viewport::resetCamera()
		{
			camera.reset();
		}

		void Viewport::toggleLight()
		{
			lightEnabled ^= true;
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
	}
}

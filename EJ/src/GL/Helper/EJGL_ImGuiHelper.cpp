#include "EJ/GL/Helper/EJGL_ImGuiHelper.hpp"
#include "EJ/GL/Camera/EJGL_Camera.hpp"
#include "EJ/Utils/EJ_Math.hpp"

#include <imgui/imgui_impl_glfwpp.hpp>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

EJGL_NAMESPACE_BEGIN

namespace ImGuiHelper {
	namespace impl {
		void imgui_init(glfw::Window& window_, const char* version_) {
			IMGUI_CHECKVERSION();
			::ImGui::CreateContext();
			ImGui_ImplGlfwpp_InitForOpenGL(window_, true);
			ImGui_ImplOpenGL3_Init(version_);
		}
		void imgui_shutdown() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfwpp_Shutdown();
			::ImGui::DestroyContext();
		}
		void imgui_newFrame() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfwpp_NewFrame();
			::ImGui::NewFrame();
		}
		void imgui_render() {
			::ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
		}
	}

	bool SliderAngle3(const char* label, float v_rads[3], float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags) {
		if (format == NULL)
			format = "%.0f deg";
		float v_degs[3];
		_STD transform(v_rads, v_rads + 3, v_degs, [](float v_) {
			return ::glm::degrees(v_);
			});
		bool value_changed = ::ImGui::SliderFloat3(label, v_degs, v_degrees_min, v_degrees_max, format, flags);
		if (value_changed) {
			v_rads[0] = ::glm::radians(v_degs[0]);
			v_rads[1] = ::glm::radians(v_degs[1]);
			v_rads[2] = ::glm::radians(v_degs[2]);
		}
		return value_changed;
	}

	bool SliderAngle3(const char* label, ::glm::quat& quat, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags) {
		if (format == NULL)
			format = "%.0f deg";
		::glm::vec3 angles = ::glm::eulerAngles(quat);
		bool value_changed = SliderAngle3(label, ::glm::value_ptr(angles), v_degrees_min, v_degrees_max, format, flags);
		if (value_changed) {
			quat = ::glm::quat(angles);
		}
		return value_changed;
	}

	bool CameraRotation(const char* label_, ::glm::quat& rotation_) {
		return SliderAngle3(label_, rotation_, -180.f, 180.f);
	}
}

EJGL_NAMESPACE_END
#ifndef EJGL_IMGUIHELPER_HPP
#define EJGL_IMGUIHELPER_HPP

#include "../Utils/EJGL_config.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/quaternion.hpp>

namespace glfw {
	class Window;
}

EJGL_NAMESPACE_BEGIN

class Camera;

namespace ImGuiHelper {
	namespace impl {
		void imgui_init(glfw::Window& window_, const char* version_);
		void imgui_shutdown();
		void imgui_newFrame();
		void imgui_render();
	}
	class ImGuiWindowLifeHandler {
	private:
		ImGuiWindowLifeHandler(const char* name_, bool* pOpen_, ImGuiWindowFlags flags_) :
			_windowRet(ImGui::Begin(name_, pOpen_, flags_))
		{}

	public:
		~ImGuiWindowLifeHandler() {
			ImGui::End();
		}

		operator bool() const {
			return _windowRet;
		}

	private:
		bool _windowRet;

	private:
		[[nodiscard]] friend ImGuiWindowLifeHandler createWindow(const char* name_, bool* pOpen_, ImGuiWindowFlags flags_)
		{
			return ImGuiWindowLifeHandler{ name_, pOpen_, flags_ };
		}

	};
	class ImGuiLifeHandler {
	private:
		ImGuiLifeHandler(glfw::Window& window_, const char* version_) {
			impl::imgui_init(window_, version_);
		}

	public:
		~ImGuiLifeHandler() {
			impl::imgui_shutdown();
		}

		void newFrame() const {
			impl::imgui_newFrame();
		}

		void render() const {
			impl::imgui_render();
		}

		[[nodiscard]] friend ImGuiLifeHandler init(glfw::Window& window_, const char* version_)
		{
			return ImGuiLifeHandler{ window_, version_ };
		}
	};
	[[nodiscard]] inline ImGuiWindowLifeHandler createWindow(const char* name_, bool* pOpen_ = nullptr, ImGuiWindowFlags flags_ = 0);
	[[nodiscard]] inline ImGuiLifeHandler init(glfw::Window& window_, const char* version_ = "#version 460");

	bool SliderAngle3(const char* label, float v_rads[3], float v_degrees_min, float v_degrees_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool SliderAngle3(const char* label, ::glm::quat& quat, float v_degrees_min, float v_degrees_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool CameraRotation(const char* label_, ::glm::quat& rotation_);
}

EJGL_NAMESPACE_END

#endif // EJGL_IMGUIHELPER_HPP
#ifndef IMGUI_HELPER_HPP
#define IMGUI_HELPER_HPP

#include <imgui/imgui.h>

namespace glfw {
	class Window;
}

namespace ImGuiHelper {
	namespace impl {
		void imgui_init(glfw::Window& window_, const char* version_);
		void imgui_shutdown();
		void imgui_newFrame();
		void imgui_render();
	}
	struct ImGuiLifeHandler {
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
	[[nodiscard]] inline ImGuiLifeHandler init(glfw::Window& window_, const char* version_ = "#version 460");
}

#endif // IMGUI_HELPER_HPP
#include "ImGuiHelper.hpp"
#include <imgui/imgui_impl_glfwpp.hpp>
#include <imgui/imgui_impl_opengl3.h>

namespace ImGuiHelper {
	namespace impl {
		void imgui_init(glfw::Window& window_, const char* version_) {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui_ImplGlfwpp_InitForOpenGL(window_, true);
			ImGui_ImplOpenGL3_Init(version_);
		}
		void imgui_shutdown() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfwpp_Shutdown();
			ImGui::DestroyContext();
		}
		void imgui_newFrame() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfwpp_NewFrame();
			ImGui::NewFrame();
		}
		void imgui_render() {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	}
}
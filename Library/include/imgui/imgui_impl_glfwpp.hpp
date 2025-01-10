// dear imgui: Platform Backend for GLFW
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan, WebGPU..)
// (Info: GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (Requires: GLFW 3.1+. Prefer GLFW 3.3+ for full feature support.)

// Implemented features:
//  [X] Platform: Clipboard support.
//  [X] Platform: Mouse support. Can discriminate Mouse/TouchScreen/Pen (Windows only).
//  [X] Platform: Keyboard support. Since 1.87 we are using the io.AddKeyEvent() function. Pass ImGuiKey values to all key functions e.g. ImGui::IsKeyPressed(ImGuiKey_Space). [Legacy GLFW_KEY_* values are obsolete since 1.87 and not supported since 1.91.5]
//  [X] Platform: Gamepad support. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [X] Platform: Mouse cursor shape and visibility (ImGuiBackendFlags_HasMouseCursors). Resizing cursors requires GLFW 3.4+! Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.
//  [X] Platform: Multi-viewport support (multiple windows). Enable with 'io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable'.
// Missing features or Issues:
//  [ ] Platform: Multi-viewport: ParentViewportID not honored, and so io.ConfigViewportsNoDefaultParent has no effect (minor).

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API
#ifndef IMGUI_DISABLE

#include <glfwpp/Monitor.hpp>

struct GLFWwindow;
namespace glfw {
	class Window;
	enum class MouseButton;
	enum class MouseButtonState;
	enum class ModifierKeyBit;
	enum class KeyCode;
	enum class KeyState;
}

// Follow "Getting Started" link and check examples/ folder to learn about using backends!
IMGUI_IMPL_API bool     ImGui_ImplGlfwpp_InitForOpenGL(glfw::Window& window, bool install_callbacks);
IMGUI_IMPL_API bool     ImGui_ImplGlfwpp_InitForVulkan(glfw::Window& window, bool install_callbacks);
IMGUI_IMPL_API bool     ImGui_ImplGlfwpp_InitForOther(glfw::Window& window, bool install_callbacks);
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_NewFrame();

// Emscripten related initialization phase methods (call after ImGui_ImplGlfw_InitForOpenGL)
#ifdef __EMSCRIPTEN__
IMGUI_IMPL_API void     ImGui_ImplGlfw_InstallEmscriptenCallbacks(GLFWwindow* window, const char* canvas_selector);
//static inline void    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback(const char* canvas_selector) { ImGui_ImplGlfw_InstallEmscriptenCallbacks(nullptr, canvas_selector); } } // Renamed in 1.91.0
#endif

// GLFW callbacks install
// - When calling Init with 'install_callbacks=true': ImGui_ImplGlfw_InstallCallbacks() is called. GLFW callbacks will be installed for you. They will chain-call user's previously installed callbacks, if any.
// - When calling Init with 'install_callbacks=false': GLFW callbacks won't be installed. You will need to call individual function yourself from your own GLFW callbacks.
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_InstallCallbacks(glfw::Window& window);
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_RestoreCallbacks(glfw::Window& window);

// GLFW callbacks (individual callbacks to call yourself if you didn't install callbacks)
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_WindowFocusCallback(glfw::Window& window, bool focused);
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_CursorEnterCallback(glfw::Window& window, bool entered);
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_CursorPosCallback(glfw::Window& window, double x, double y);
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_MouseButtonCallback(glfw::Window& window, glfw::MouseButton button, glfw::MouseButtonState action, glfw::ModifierKeyBit mods);
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_ScrollCallback(glfw::Window& window, double xoffset, double yoffset);
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_KeyCallback(glfw::Window& window, glfw::KeyCode keycode, int scancode, glfw::KeyState action, glfw::ModifierKeyBit mods);
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_CharCallback(glfw::Window& window, unsigned int c);
IMGUI_IMPL_API void     ImGui_ImplGlfwpp_MonitorCallback(glfw::Monitor monitor, glfw::Monitor::EventType event);

// GLFW helpers
IMGUI_IMPL_API void     ImGui_ImplGlfw_Sleep(int milliseconds);

#endif // #ifndef IMGUI_DISABLE

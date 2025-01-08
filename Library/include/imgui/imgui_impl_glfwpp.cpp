#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_glfwpp.hpp"

// Clang warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"     // warning: use of old-style cast
#pragma clang diagnostic ignored "-Wsign-conversion"    // warning: implicit conversion changes signedness
#endif

// GLFW
#include <vector>
#include <glfwpp/glfw.hpp>

#ifdef _WIN32
#undef APIENTRY
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>   // for glfwGetWin32Window()
#endif
#ifndef _WIN32
#include <unistd.h>             // for usleep()
#endif

// We gather version tests as define in order to easily see which features are version-dependent.
#define GLFW_VERSION_COMBINED           (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 + GLFW_VERSION_REVISION)
#define GLFW_HAS_WINDOW_TOPMOST         (GLFW_VERSION_COMBINED >= 3200) // 3.2+ GLFW_FLOATING
#define GLFW_HAS_WINDOW_HOVERED         (GLFW_VERSION_COMBINED >= 3300) // 3.3+ GLFW_HOVERED
#define GLFW_HAS_WINDOW_ALPHA           (GLFW_VERSION_COMBINED >= 3300) // 3.3+ glfwSetWindowOpacity
#define GLFW_HAS_PER_MONITOR_DPI        (GLFW_VERSION_COMBINED >= 3300) // 3.3+ glfwGetMonitorContentScale
#if defined(__EMSCRIPTEN__) || defined(__SWITCH__)                      // no Vulkan support in GLFW for Emscripten or homebrew Nintendo Switch
#define GLFW_HAS_VULKAN                 (0)
#else
#define GLFW_HAS_VULKAN                 (GLFW_VERSION_COMBINED >= 3200) // 3.2+ glfwCreateWindowSurface
#endif
#define GLFW_HAS_FOCUS_WINDOW           (GLFW_VERSION_COMBINED >= 3200) // 3.2+ glfwFocusWindow
#define GLFW_HAS_FOCUS_ON_SHOW          (GLFW_VERSION_COMBINED >= 3300) // 3.3+ GLFW_FOCUS_ON_SHOW
#define GLFW_HAS_MONITOR_WORK_AREA      (GLFW_VERSION_COMBINED >= 3300) // 3.3+ glfwGetMonitorWorkarea
#define GLFW_HAS_OSX_WINDOW_POS_FIX     (GLFW_VERSION_COMBINED >= 3301) // 3.3.1+ Fixed: Resizing window repositions it on MacOS #1553
#ifdef GLFW_RESIZE_NESW_CURSOR          // Let's be nice to people who pulled GLFW between 2019-04-16 (3.4 define) and 2019-11-29 (cursors defines) // FIXME: Remove when GLFW 3.4 is released?
#define GLFW_HAS_NEW_CURSORS            (GLFW_VERSION_COMBINED >= 3400) // 3.4+ GLFW_RESIZE_ALL_CURSOR, GLFW_RESIZE_NESW_CURSOR, GLFW_RESIZE_NWSE_CURSOR, GLFW_NOT_ALLOWED_CURSOR
#else
#define GLFW_HAS_NEW_CURSORS            (0)
#endif
#ifdef GLFW_MOUSE_PASSTHROUGH           // Let's be nice to people who pulled GLFW between 2019-04-16 (3.4 define) and 2020-07-17 (passthrough)
#define GLFW_HAS_MOUSE_PASSTHROUGH      (GLFW_VERSION_COMBINED >= 3400) // 3.4+ GLFW_MOUSE_PASSTHROUGH
#else
#define GLFW_HAS_MOUSE_PASSTHROUGH      (0)
#endif
#define GLFW_HAS_GAMEPAD_API            (GLFW_VERSION_COMBINED >= 3300) // 3.3+ glfwGetGamepadState() new api
#define GLFW_HAS_GETKEYNAME             (GLFW_VERSION_COMBINED >= 3200) // 3.2+ glfwGetKeyName()
#define GLFW_HAS_GETERROR               (GLFW_VERSION_COMBINED >= 3300) // 3.3+ glfwGetError()

// GLFW data
enum GlfwClientApi
{
	GlfwClientApi_Unknown,
	GlfwClientApi_OpenGL,
	GlfwClientApi_Vulkan,
};

struct ImGui_ImplGlfw_Data
{
	GLFWwindow*				Window = nullptr;
	GlfwClientApi           ClientApi = GlfwClientApi_Unknown;
	double                  Time = 0.0;
	GLFWwindow*				MouseWindow = nullptr;
	GLFWcursor*				MouseCursors[ImGuiMouseCursor_COUNT] = {};
	bool                    MouseIgnoreButtonUpWaitForFocusLoss = false;
	bool                    MouseIgnoreButtonUp = false;
	ImVec2                  LastValidMousePos = {};
	GLFWwindow*				KeyOwnerWindows[GLFW_KEY_LAST] = {};
	bool                    InstalledCallbacks = false;
	bool                    WantUpdateMonitors = false;
#ifdef EMSCRIPTEN_USE_EMBEDDED_GLFW3
	const char* CanvasSelector;
#endif

#ifdef _WIN32
	WNDPROC                 PrevWndProc = 0;
#endif

	std::vector<CallMe::Subscription> Subscriptions = {};

	ImGui_ImplGlfw_Data() {}
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
// FIXME: multi-context support is not well tested and probably dysfunctional in this backend.
// - Because glfwPollEvents() process all windows and some events may be called outside of it, you will need to register your own callbacks
//   (passing install_callbacks=false in ImGui_ImplGlfw_InitXXX functions), set the current dear imgui context and then call our callbacks.
// - Otherwise we may need to store a GLFWwindow* -> ImGuiContext* map and handle this in the backend, adding a little bit of extra complexity to it.
// FIXME: some shared resources (mouse cursor shape, gamepad) are mishandled when using multi-context.
static ImGui_ImplGlfw_Data* ImGui_ImplGlfw_GetBackendData()
{
	return ImGui::GetCurrentContext() ? (ImGui_ImplGlfw_Data*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

// Forward Declarations
static void ImGui_ImplGlfw_UpdateMonitors();
static void ImGui_ImplGlfw_InitMultiViewportSupport();
static void ImGui_ImplGlfw_ShutdownMultiViewportSupport();

// Functions

// Not static to allow third-party code to use that if they want to (but undocumented)
ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode);
ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode)
{
	IM_UNUSED(scancode);
	switch (keycode)
	{
	case GLFW_KEY_TAB: return ImGuiKey_Tab;
	case GLFW_KEY_LEFT: return ImGuiKey_LeftArrow;
	case GLFW_KEY_RIGHT: return ImGuiKey_RightArrow;
	case GLFW_KEY_UP: return ImGuiKey_UpArrow;
	case GLFW_KEY_DOWN: return ImGuiKey_DownArrow;
	case GLFW_KEY_PAGE_UP: return ImGuiKey_PageUp;
	case GLFW_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
	case GLFW_KEY_HOME: return ImGuiKey_Home;
	case GLFW_KEY_END: return ImGuiKey_End;
	case GLFW_KEY_INSERT: return ImGuiKey_Insert;
	case GLFW_KEY_DELETE: return ImGuiKey_Delete;
	case GLFW_KEY_BACKSPACE: return ImGuiKey_Backspace;
	case GLFW_KEY_SPACE: return ImGuiKey_Space;
	case GLFW_KEY_ENTER: return ImGuiKey_Enter;
	case GLFW_KEY_ESCAPE: return ImGuiKey_Escape;
	case GLFW_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
	case GLFW_KEY_COMMA: return ImGuiKey_Comma;
	case GLFW_KEY_MINUS: return ImGuiKey_Minus;
	case GLFW_KEY_PERIOD: return ImGuiKey_Period;
	case GLFW_KEY_SLASH: return ImGuiKey_Slash;
	case GLFW_KEY_SEMICOLON: return ImGuiKey_Semicolon;
	case GLFW_KEY_EQUAL: return ImGuiKey_Equal;
	case GLFW_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
	case GLFW_KEY_BACKSLASH: return ImGuiKey_Backslash;
	case GLFW_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
	case GLFW_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
	case GLFW_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
	case GLFW_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
	case GLFW_KEY_NUM_LOCK: return ImGuiKey_NumLock;
	case GLFW_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
	case GLFW_KEY_PAUSE: return ImGuiKey_Pause;
	case GLFW_KEY_KP_0: return ImGuiKey_Keypad0;
	case GLFW_KEY_KP_1: return ImGuiKey_Keypad1;
	case GLFW_KEY_KP_2: return ImGuiKey_Keypad2;
	case GLFW_KEY_KP_3: return ImGuiKey_Keypad3;
	case GLFW_KEY_KP_4: return ImGuiKey_Keypad4;
	case GLFW_KEY_KP_5: return ImGuiKey_Keypad5;
	case GLFW_KEY_KP_6: return ImGuiKey_Keypad6;
	case GLFW_KEY_KP_7: return ImGuiKey_Keypad7;
	case GLFW_KEY_KP_8: return ImGuiKey_Keypad8;
	case GLFW_KEY_KP_9: return ImGuiKey_Keypad9;
	case GLFW_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
	case GLFW_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
	case GLFW_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
	case GLFW_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
	case GLFW_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
	case GLFW_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
	case GLFW_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
	case GLFW_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
	case GLFW_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
	case GLFW_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
	case GLFW_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
	case GLFW_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
	case GLFW_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
	case GLFW_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
	case GLFW_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
	case GLFW_KEY_MENU: return ImGuiKey_Menu;
	case GLFW_KEY_0: return ImGuiKey_0;
	case GLFW_KEY_1: return ImGuiKey_1;
	case GLFW_KEY_2: return ImGuiKey_2;
	case GLFW_KEY_3: return ImGuiKey_3;
	case GLFW_KEY_4: return ImGuiKey_4;
	case GLFW_KEY_5: return ImGuiKey_5;
	case GLFW_KEY_6: return ImGuiKey_6;
	case GLFW_KEY_7: return ImGuiKey_7;
	case GLFW_KEY_8: return ImGuiKey_8;
	case GLFW_KEY_9: return ImGuiKey_9;
	case GLFW_KEY_A: return ImGuiKey_A;
	case GLFW_KEY_B: return ImGuiKey_B;
	case GLFW_KEY_C: return ImGuiKey_C;
	case GLFW_KEY_D: return ImGuiKey_D;
	case GLFW_KEY_E: return ImGuiKey_E;
	case GLFW_KEY_F: return ImGuiKey_F;
	case GLFW_KEY_G: return ImGuiKey_G;
	case GLFW_KEY_H: return ImGuiKey_H;
	case GLFW_KEY_I: return ImGuiKey_I;
	case GLFW_KEY_J: return ImGuiKey_J;
	case GLFW_KEY_K: return ImGuiKey_K;
	case GLFW_KEY_L: return ImGuiKey_L;
	case GLFW_KEY_M: return ImGuiKey_M;
	case GLFW_KEY_N: return ImGuiKey_N;
	case GLFW_KEY_O: return ImGuiKey_O;
	case GLFW_KEY_P: return ImGuiKey_P;
	case GLFW_KEY_Q: return ImGuiKey_Q;
	case GLFW_KEY_R: return ImGuiKey_R;
	case GLFW_KEY_S: return ImGuiKey_S;
	case GLFW_KEY_T: return ImGuiKey_T;
	case GLFW_KEY_U: return ImGuiKey_U;
	case GLFW_KEY_V: return ImGuiKey_V;
	case GLFW_KEY_W: return ImGuiKey_W;
	case GLFW_KEY_X: return ImGuiKey_X;
	case GLFW_KEY_Y: return ImGuiKey_Y;
	case GLFW_KEY_Z: return ImGuiKey_Z;
	case GLFW_KEY_F1: return ImGuiKey_F1;
	case GLFW_KEY_F2: return ImGuiKey_F2;
	case GLFW_KEY_F3: return ImGuiKey_F3;
	case GLFW_KEY_F4: return ImGuiKey_F4;
	case GLFW_KEY_F5: return ImGuiKey_F5;
	case GLFW_KEY_F6: return ImGuiKey_F6;
	case GLFW_KEY_F7: return ImGuiKey_F7;
	case GLFW_KEY_F8: return ImGuiKey_F8;
	case GLFW_KEY_F9: return ImGuiKey_F9;
	case GLFW_KEY_F10: return ImGuiKey_F10;
	case GLFW_KEY_F11: return ImGuiKey_F11;
	case GLFW_KEY_F12: return ImGuiKey_F12;
	case GLFW_KEY_F13: return ImGuiKey_F13;
	case GLFW_KEY_F14: return ImGuiKey_F14;
	case GLFW_KEY_F15: return ImGuiKey_F15;
	case GLFW_KEY_F16: return ImGuiKey_F16;
	case GLFW_KEY_F17: return ImGuiKey_F17;
	case GLFW_KEY_F18: return ImGuiKey_F18;
	case GLFW_KEY_F19: return ImGuiKey_F19;
	case GLFW_KEY_F20: return ImGuiKey_F20;
	case GLFW_KEY_F21: return ImGuiKey_F21;
	case GLFW_KEY_F22: return ImGuiKey_F22;
	case GLFW_KEY_F23: return ImGuiKey_F23;
	case GLFW_KEY_F24: return ImGuiKey_F24;
	default: return ImGuiKey_None;
	}
}

// X11 does not include current pressed/released modifier key in 'mods' flags submitted by GLFW
// See https://github.com/ocornut/imgui/issues/6034 and https://github.com/glfw/glfw/issues/1630
static void ImGui_ImplGlfw_UpdateKeyModifiers(GLFWwindow* window)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddKeyEvent(ImGuiMod_Ctrl,  (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
	io.AddKeyEvent(ImGuiMod_Shift, (glfwGetKey(window,   GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window,   GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS));
	io.AddKeyEvent(ImGuiMod_Alt,   (glfwGetKey(window,     GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window,     GLFW_KEY_RIGHT_ALT) == GLFW_PRESS));
	io.AddKeyEvent(ImGuiMod_Super, (glfwGetKey(window,   GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window,   GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS));
}

void ImGui_ImplGlfwpp_MouseButtonCallback(glfw::Window& window, glfw::MouseButton button, glfw::MouseButtonState action, glfw::ModifierKeyBit)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();

	// Workaround for Linux: ignore mouse up events which are following an focus loss following a viewport creation
	if (bd->MouseIgnoreButtonUp && action == glfw::MouseButtonState::Release)
		return;

	ImGui_ImplGlfw_UpdateKeyModifiers(window);

	ImGuiIO& io = ImGui::GetIO();
	if (static_cast<int>(button) >= 0 && static_cast<int>(button) < ImGuiMouseButton_COUNT)
		io.AddMouseButtonEvent(static_cast<int>(button), action == glfw::MouseButtonState::Press);
}

void ImGui_ImplGlfwpp_ScrollCallback(glfw::Window& window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();

#ifdef EMSCRIPTEN_USE_EMBEDDED_GLFW3
	// Ignore GLFW events: will be processed in ImGui_ImplEmscripten_WheelCallback().
	return;
#endif

	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
}

// FIXME: should this be baked into ImGui_ImplGlfw_KeyToImGuiKey()? then what about the values passed to io.SetKeyEventNativeData()?
static int ImGui_ImplGlfw_TranslateUntranslatedKey(int key, int scancode)
{
#if GLFW_HAS_GETKEYNAME && !defined(EMSCRIPTEN_USE_EMBEDDED_GLFW3)
	// GLFW 3.1+ attempts to "untranslate" keys, which goes the opposite of what every other framework does, making using lettered shortcuts difficult.
	// (It had reasons to do so: namely GLFW is/was more likely to be used for WASD-type game controls rather than lettered shortcuts, but IHMO the 3.1 change could have been done differently)
	// See https://github.com/glfw/glfw/issues/1502 for details.
	// Adding a workaround to undo this (so our keys are translated->untranslated->translated, likely a lossy process).
	// This won't cover edge cases but this is at least going to cover common cases.
	if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_EQUAL)
		return key;
	GLFWerrorfun prev_error_callback = glfwSetErrorCallback(nullptr);
	const char* key_name = glfwGetKeyName(key, scancode);
	glfwSetErrorCallback(prev_error_callback);
#if GLFW_HAS_GETERROR && !defined(EMSCRIPTEN_USE_EMBEDDED_GLFW3) // Eat errors (see #5908)
	(void)glfwGetError(nullptr);
#endif
	if (key_name && key_name[0] != 0 && key_name[1] == 0)
	{
		const char char_names[] = "`-=[]\\,;\'./";
		const int char_keys[] = { GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_MINUS, GLFW_KEY_EQUAL, GLFW_KEY_LEFT_BRACKET, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_BACKSLASH, GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_APOSTROPHE, GLFW_KEY_PERIOD, GLFW_KEY_SLASH, 0 };
		IM_ASSERT(IM_ARRAYSIZE(char_names) == IM_ARRAYSIZE(char_keys));
		if (key_name[0] >= '0' && key_name[0] <= '9') { key = GLFW_KEY_0 + (key_name[0] - '0'); }
		else if (key_name[0] >= 'A' && key_name[0] <= 'Z') { key = GLFW_KEY_A + (key_name[0] - 'A'); }
		else if (key_name[0] >= 'a' && key_name[0] <= 'z') { key = GLFW_KEY_A + (key_name[0] - 'a'); }
		else if (const char* p = strchr(char_names, key_name[0])) { key = char_keys[p - char_names]; }
	}
	// if (action == GLFW_PRESS) printf("key %d scancode %d name '%s'\n", key, scancode, key_name);
#else
	IM_UNUSED(scancode);
#endif
	return key;
}

void ImGui_ImplGlfwpp_KeyCallback(glfw::Window& window, glfw::KeyCode keycode, int scancode, glfw::KeyState action, glfw::ModifierKeyBit mods)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();

	if (action != glfw::KeyState::Press && action != glfw::KeyState::Release)
		return;

	ImGui_ImplGlfw_UpdateKeyModifiers(window);

	if (static_cast<int>(keycode) >= 0 && static_cast<int>(keycode) < IM_ARRAYSIZE(bd->KeyOwnerWindows))
		bd->KeyOwnerWindows[static_cast<int>(keycode)] = (action == glfw::KeyState::Press) ? window : nullptr;

	int newKeycode = ImGui_ImplGlfw_TranslateUntranslatedKey(static_cast<int>(keycode), scancode);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(newKeycode, scancode);
	io.AddKeyEvent(imgui_key, (action == glfw::KeyState::Press));
	io.SetKeyEventNativeData(imgui_key, newKeycode, scancode); // To support legacy indexing (<1.87 user code)
}

void ImGui_ImplGlfwpp_WindowFocusCallback(glfw::Window& window, bool focused)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();

	// Workaround for Linux: when losing focus with MouseIgnoreButtonUpWaitForFocusLoss set, we will temporarily ignore subsequent Mouse Up events
	bd->MouseIgnoreButtonUp = (bd->MouseIgnoreButtonUpWaitForFocusLoss && !focused);
	bd->MouseIgnoreButtonUpWaitForFocusLoss = false;

	ImGuiIO& io = ImGui::GetIO();
	io.AddFocusEvent(focused);
}

void ImGui_ImplGlfwpp_CursorPosCallback(glfw::Window& window, double x, double y)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		int window_x, window_y;
		glfwGetWindowPos(window, &window_x, &window_y);
		x += window_x;
		y += window_y;
	}
	io.AddMousePosEvent((float)x, (float)y);
	bd->LastValidMousePos = ImVec2((float)x, (float)y);
}

// Workaround: X11 seems to send spurious Leave/Enter events which would make us lose our position,
// so we back it up and restore on Leave/Enter (see https://github.com/ocornut/imgui/issues/4984)
void ImGui_ImplGlfwpp_CursorEnterCallback(glfw::Window& window, bool entered)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();

	ImGuiIO& io = ImGui::GetIO();
	if (entered)
	{
		bd->MouseWindow = window;
		io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
	}
	else if (bd->MouseWindow == window)
	{
		bd->LastValidMousePos = io.MousePos;
		bd->MouseWindow = nullptr;
		io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
	}
}

void ImGui_ImplGlfwpp_CharCallback(glfw::Window& window, unsigned int c)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();

	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(c);
}

void ImGui_ImplGlfwpp_MonitorCallback(glfw::Monitor, glfw::Monitor::EventType)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	bd->WantUpdateMonitors = true;
}

#ifdef EMSCRIPTEN_USE_EMBEDDED_GLFW3
static EM_BOOL ImGui_ImplEmscripten_WheelCallback(int, const EmscriptenWheelEvent* ev, void*)
{
	// Mimic Emscripten_HandleWheel() in SDL.
	// Corresponding equivalent in GLFW JS emulation layer has incorrect quantizing preventing small values. See #6096
	float multiplier = 0.0f;
	if (ev->deltaMode == DOM_DELTA_PIXEL) { multiplier = 1.0f / 100.0f; } // 100 pixels make up a step.
	else if (ev->deltaMode == DOM_DELTA_LINE) { multiplier = 1.0f / 3.0f; }   // 3 lines make up a step.
	else if (ev->deltaMode == DOM_DELTA_PAGE) { multiplier = 80.0f; }         // A page makes up 80 steps.
	float wheel_x = ev->deltaX * -multiplier;
	float wheel_y = ev->deltaY * -multiplier;
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent(wheel_x, wheel_y);
	//IMGUI_DEBUG_LOG("[Emsc] mode %d dx: %.2f, dy: %.2f, dz: %.2f --> feed %.2f %.2f\n", (int)ev->deltaMode, ev->deltaX, ev->deltaY, ev->deltaZ, wheel_x, wheel_y);
	return EM_TRUE;
}
#endif

#ifdef _WIN32
static LRESULT CALLBACK ImGui_ImplGlfw_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

void ImGui_ImplGlfwpp_InstallCallbacks(glfw::Window& window)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	IM_ASSERT(bd->InstalledCallbacks == false && "Callbacks already installed!");
	IM_ASSERT(bd->Window == window);

	bd->Subscriptions.reserve(8);
	bd->Subscriptions.emplace_back(window.focusEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_WindowFocusCallback>()));
	bd->Subscriptions.emplace_back(window.cursorEnterEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_CursorEnterCallback>()));
	bd->Subscriptions.emplace_back(window.cursorPosEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_CursorPosCallback>()));
	bd->Subscriptions.emplace_back(window.mouseButtonEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_MouseButtonCallback>()));
	bd->Subscriptions.emplace_back(window.scrollEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_ScrollCallback>()));
	bd->Subscriptions.emplace_back(window.keyEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_KeyCallback>()));
	bd->Subscriptions.emplace_back(window.charEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_CharCallback>()));
	bd->Subscriptions.emplace_back(glfw::Monitor::monitorEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_MonitorCallback>()));
	bd->InstalledCallbacks = true;
}

void ImGui_ImplGlfwpp_RestoreCallbacks(glfw::Window& window)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	IM_ASSERT(bd->InstalledCallbacks == true && "Callbacks not installed!");
	IM_ASSERT(bd->Window == window);

	bd->Subscriptions.clear();
	bd->InstalledCallbacks = false;
}

#ifdef __EMSCRIPTEN__
#if EMSCRIPTEN_USE_PORT_CONTRIB_GLFW3 >= 34020240817
void ImGui_ImplGlfw_EmscriptenOpenURL(const char* url) { if (url) emscripten::glfw3::OpenURL(url); }
#else
EM_JS(void, ImGui_ImplGlfw_EmscriptenOpenURL, (const char* url), { url = url ? UTF8ToString(url) : null; if (url) window.open(url, '_blank'); });
#endif
#endif

static bool ImGui_ImplGlfw_Init(glfw::Window& window, bool install_callbacks, GlfwClientApi client_api)
{
	ImGuiIO& io = ImGui::GetIO();
	IMGUI_CHECKVERSION();
	IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");
	//printf("GLFW_VERSION: %d.%d.%d (%d)", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION, GLFW_VERSION_COMBINED);

	// Setup backend capabilities flags
	ImGui_ImplGlfw_Data* bd = IM_NEW(ImGui_ImplGlfw_Data)();
	io.BackendPlatformUserData = (void*)bd;
	io.BackendPlatformName = "imgui_impl_glfw";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
#ifndef __EMSCRIPTEN__
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
#endif
#if GLFW_HAS_MOUSE_PASSTHROUGH || GLFW_HAS_WINDOW_HOVERED
	io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can call io.AddMouseViewportEvent() with correct data (optional)
#endif

	bd->Window = window;
	bd->Time = 0.0;
	bd->WantUpdateMonitors = true;

	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Platform_SetClipboardTextFn = [](ImGuiContext*, const char* text) { glfwSetClipboardString(nullptr, text); };
	platform_io.Platform_GetClipboardTextFn = [](ImGuiContext*) { return glfwGetClipboardString(nullptr); };
#ifdef __EMSCRIPTEN__
	platform_io.Platform_OpenInShellFn = [](ImGuiContext*, const char* url) { ImGui_ImplGlfw_EmscriptenOpenURL(url); return true; };
#endif

	// Create mouse cursors
	// (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
	// GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
	// Missing cursors will return nullptr and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
	GLFWerrorfun prev_error_callback = glfwSetErrorCallback(nullptr);
	bd->MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
	bd->MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
	bd->MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
	glfwSetErrorCallback(prev_error_callback);
#if GLFW_HAS_GETERROR && !defined(__EMSCRIPTEN__) // Eat errors (see #5908)
	(void)glfwGetError(nullptr);
#endif

	// Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
	if (install_callbacks)
		ImGui_ImplGlfwpp_InstallCallbacks(window);

	// Update monitor a first time during init
	// (note: monitor callback are broken in GLFW 3.2 and earlier, see github.com/glfw/glfw/issues/784)
	ImGui_ImplGlfw_UpdateMonitors();
	// have install in InstallCallbacks
	if (!install_callbacks) {
		bd->Subscriptions.emplace_back(glfw::Monitor::monitorEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_MonitorCallback>()));
	}

	// Set platform dependent data in viewport
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	main_viewport->PlatformHandle = (void*)bd->Window;
#ifdef _WIN32
	main_viewport->PlatformHandleRaw = glfwGetWin32Window(bd->Window);
#elif defined(__APPLE__)
	main_viewport->PlatformHandleRaw = (void*)glfwGetCocoaWindow(bd->Window);
#else
	IM_UNUSED(main_viewport);
#endif
	ImGui_ImplGlfw_InitMultiViewportSupport();

	// Windows: register a WndProc hook so we can intercept some messages.
#ifdef _WIN32
	bd->PrevWndProc = (WNDPROC)::GetWindowLongPtrW((HWND)main_viewport->PlatformHandleRaw, GWLP_WNDPROC);
	IM_ASSERT(bd->PrevWndProc != nullptr);
	::SetWindowLongPtrW((HWND)main_viewport->PlatformHandleRaw, GWLP_WNDPROC, (LONG_PTR)ImGui_ImplGlfw_WndProc);
#endif

	// Emscripten: the same application can run on various platforms, so we detect the Apple platform at runtime
	// to override io.ConfigMacOSXBehaviors from its default (which is always false in Emscripten).
#ifdef __EMSCRIPTEN__
#if EMSCRIPTEN_USE_PORT_CONTRIB_GLFW3 >= 34020240817
	if (emscripten::glfw3::IsRuntimePlatformApple())
	{
		ImGui::GetIO().ConfigMacOSXBehaviors = true;

		// Due to how the browser (poorly) handles the Meta Key, this line essentially disables repeats when used.
		// This means that Meta + V only registers a single key-press, even if the keys are held.
		// This is a compromise for dealing with this issue in ImGui since ImGui implements key repeat itself.
		// See https://github.com/pongasoft/emscripten-glfw/blob/v3.4.0.20240817/docs/Usage.md#the-problem-of-the-super-key
		emscripten::glfw3::SetSuperPlusKeyTimeouts(10, 10);
	}
#endif
#endif

	bd->ClientApi = client_api;
	return true;
}

bool ImGui_ImplGlfwpp_InitForOpenGL(glfw::Window& window, bool install_callbacks)
{
	return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_OpenGL);
}

bool ImGui_ImplGlfwpp_InitForVulkan(glfw::Window& window, bool install_callbacks)
{
	return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_Vulkan);
}

bool ImGui_ImplGlfwpp_InitForOther(glfw::Window& window, bool install_callbacks)
{
	return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_Unknown);
}

void ImGui_ImplGlfwpp_Shutdown()
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplGlfw_ShutdownMultiViewportSupport();

	if (bd->InstalledCallbacks)
		ImGui_ImplGlfwpp_RestoreCallbacks(glfw::Window::getWrapperFromHandle(bd->Window));
#ifdef EMSCRIPTEN_USE_EMBEDDED_GLFW3
	if (bd->CanvasSelector)
		emscripten_set_wheel_callback(bd->CanvasSelector, nullptr, false, nullptr);
#endif

	for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
		glfwDestroyCursor(bd->MouseCursors[cursor_n]);

	// Windows: restore our WndProc hook
#ifdef _WIN32
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	::SetWindowLongPtrW((HWND)main_viewport->PlatformHandleRaw, GWLP_WNDPROC, (LONG_PTR)bd->PrevWndProc);
	bd->PrevWndProc = nullptr;
#endif

	io.BackendPlatformName = nullptr;
	io.BackendPlatformUserData = nullptr;
	io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad | ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_HasMouseHoveredViewport);
	IM_DELETE(bd);
}

static void ImGui_ImplGlfw_UpdateMouseData()
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	ImGuiIO& io = ImGui::GetIO();
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

	ImGuiID mouse_viewport_id = 0;
	const ImVec2 mouse_pos_prev = io.MousePos;
	for (int n = 0; n < platform_io.Viewports.Size; n++)
	{
		ImGuiViewport* viewport = platform_io.Viewports[n];
		GLFWwindow* window = (GLFWwindow*)viewport->PlatformHandle;

#ifdef EMSCRIPTEN_USE_EMBEDDED_GLFW3
		const bool is_window_focused = true;
#else
		const bool is_window_focused = glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0;
#endif
		if (is_window_focused)
		{
			// (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when io.ConfigNavMoveSetMousePos is enabled by user)
			// When multi-viewports are enabled, all Dear ImGui positions are same as OS positions.
			if (io.WantSetMousePos)
				glfwSetCursorPos(window, (double)(mouse_pos_prev.x - viewport->Pos.x), (double)(mouse_pos_prev.y - viewport->Pos.y));

			// (Optional) Fallback to provide mouse position when focused (ImGui_ImplGlfw_CursorPosCallback already provides this when hovered or captured)
			if (bd->MouseWindow == nullptr)
			{
				double mouse_x, mouse_y;
				glfwGetCursorPos(window, &mouse_x, &mouse_y);
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					// Single viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
					// Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
					int window_x, window_y;
					glfwGetWindowPos(window, &window_x, &window_y);
					mouse_x += window_x;
					mouse_y += window_y;
				}
				bd->LastValidMousePos = ImVec2((float)mouse_x, (float)mouse_y);
				io.AddMousePosEvent((float)mouse_x, (float)mouse_y);
			}
		}

		// (Optional) When using multiple viewports: call io.AddMouseViewportEvent() with the viewport the OS mouse cursor is hovering.
		// If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, Dear imGui will ignore this field and infer the information using its flawed heuristic.
		// - [X] GLFW >= 3.3 backend ON WINDOWS ONLY does correctly ignore viewports with the _NoInputs flag (since we implement hit via our WndProc hook)
		//       On other platforms we rely on the library fallbacking to its own search when reporting a viewport with _NoInputs flag.
		// - [!] GLFW <= 3.2 backend CANNOT correctly ignore viewports with the _NoInputs flag, and CANNOT reported Hovered Viewport because of mouse capture.
		//       Some backend are not able to handle that correctly. If a backend report an hovered viewport that has the _NoInputs flag (e.g. when dragging a window
		//       for docking, the viewport has the _NoInputs flag in order to allow us to find the viewport under), then Dear ImGui is forced to ignore the value reported
		//       by the backend, and use its flawed heuristic to guess the viewport behind.
		// - [X] GLFW backend correctly reports this regardless of another viewport behind focused and dragged from (we need this to find a useful drag and drop target).
		// FIXME: This is currently only correct on Win32. See what we do below with the WM_NCHITTEST, missing an equivalent for other systems.
		// See https://github.com/glfw/glfw/issues/1236 if you want to help in making this a GLFW feature.
#if GLFW_HAS_MOUSE_PASSTHROUGH
		const bool window_no_input = (viewport->Flags & ImGuiViewportFlags_NoInputs) != 0;
		glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, window_no_input);
#endif
#if GLFW_HAS_MOUSE_PASSTHROUGH || GLFW_HAS_WINDOW_HOVERED
		if (glfwGetWindowAttrib(window, GLFW_HOVERED))
			mouse_viewport_id = viewport->ID;
#else
		// We cannot use bd->MouseWindow maintained from CursorEnter/Leave callbacks, because it is locked to the window capturing mouse.
#endif
	}

	if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)
		io.AddMouseViewportEvent(mouse_viewport_id);
}

static void ImGui_ImplGlfw_UpdateMouseCursor()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(bd->Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		return;

	ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	for (int n = 0; n < platform_io.Viewports.Size; n++)
	{
		GLFWwindow* window = (GLFWwindow*)platform_io.Viewports[n]->PlatformHandle;
		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else
		{
			// Show OS mouse cursor
			// FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
			glfwSetCursor(window, bd->MouseCursors[imgui_cursor] ? bd->MouseCursors[imgui_cursor] : bd->MouseCursors[ImGuiMouseCursor_Arrow]);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

// Update gamepad inputs
static inline float Saturate(float v) { return v < 0.0f ? 0.0f : v  > 1.0f ? 1.0f : v; }
static void ImGui_ImplGlfw_UpdateGamepads()
{
	ImGuiIO& io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
		return;

	io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
#if GLFW_HAS_GAMEPAD_API && !defined(EMSCRIPTEN_USE_EMBEDDED_GLFW3)
	GLFWgamepadstate gamepad;
	if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
		return;
#define MAP_BUTTON(KEY_NO, BUTTON_NO, _UNUSED)          do { io.AddKeyEvent(KEY_NO, gamepad.buttons[BUTTON_NO] != 0); } while (0)
#define MAP_ANALOG(KEY_NO, AXIS_NO, _UNUSED, V0, V1)    do { float v = gamepad.axes[AXIS_NO]; v = (v - V0) / (V1 - V0); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); } while (0)
#else
	int axes_count = 0, buttons_count = 0;
	const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
	const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
	if (axes_count == 0 || buttons_count == 0)
		return;
#define MAP_BUTTON(KEY_NO, _UNUSED, BUTTON_NO)          do { io.AddKeyEvent(KEY_NO, (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS)); } while (0)
#define MAP_ANALOG(KEY_NO, _UNUSED, AXIS_NO, V0, V1)    do { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); } while (0)
#endif
	io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
	MAP_BUTTON(ImGuiKey_GamepadStart, GLFW_GAMEPAD_BUTTON_START, 7);
	MAP_BUTTON(ImGuiKey_GamepadBack, GLFW_GAMEPAD_BUTTON_BACK, 6);
	MAP_BUTTON(ImGuiKey_GamepadFaceLeft, GLFW_GAMEPAD_BUTTON_X, 2);     // Xbox X, PS Square
	MAP_BUTTON(ImGuiKey_GamepadFaceRight, GLFW_GAMEPAD_BUTTON_B, 1);     // Xbox B, PS Circle
	MAP_BUTTON(ImGuiKey_GamepadFaceUp, GLFW_GAMEPAD_BUTTON_Y, 3);     // Xbox Y, PS Triangle
	MAP_BUTTON(ImGuiKey_GamepadFaceDown, GLFW_GAMEPAD_BUTTON_A, 0);     // Xbox A, PS Cross
	MAP_BUTTON(ImGuiKey_GamepadDpadLeft, GLFW_GAMEPAD_BUTTON_DPAD_LEFT, 13);
	MAP_BUTTON(ImGuiKey_GamepadDpadRight, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, 11);
	MAP_BUTTON(ImGuiKey_GamepadDpadUp, GLFW_GAMEPAD_BUTTON_DPAD_UP, 10);
	MAP_BUTTON(ImGuiKey_GamepadDpadDown, GLFW_GAMEPAD_BUTTON_DPAD_DOWN, 12);
	MAP_BUTTON(ImGuiKey_GamepadL1, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, 4);
	MAP_BUTTON(ImGuiKey_GamepadR1, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, 5);
	MAP_ANALOG(ImGuiKey_GamepadL2, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, 4, -0.75f, +1.0f);
	MAP_ANALOG(ImGuiKey_GamepadR2, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 5, -0.75f, +1.0f);
	MAP_BUTTON(ImGuiKey_GamepadL3, GLFW_GAMEPAD_BUTTON_LEFT_THUMB, 8);
	MAP_BUTTON(ImGuiKey_GamepadR3, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, 9);
	MAP_ANALOG(ImGuiKey_GamepadLStickLeft, GLFW_GAMEPAD_AXIS_LEFT_X, 0, -0.25f, -1.0f);
	MAP_ANALOG(ImGuiKey_GamepadLStickRight, GLFW_GAMEPAD_AXIS_LEFT_X, 0, +0.25f, +1.0f);
	MAP_ANALOG(ImGuiKey_GamepadLStickUp, GLFW_GAMEPAD_AXIS_LEFT_Y, 1, -0.25f, -1.0f);
	MAP_ANALOG(ImGuiKey_GamepadLStickDown, GLFW_GAMEPAD_AXIS_LEFT_Y, 1, +0.25f, +1.0f);
	MAP_ANALOG(ImGuiKey_GamepadRStickLeft, GLFW_GAMEPAD_AXIS_RIGHT_X, 2, -0.25f, -1.0f);
	MAP_ANALOG(ImGuiKey_GamepadRStickRight, GLFW_GAMEPAD_AXIS_RIGHT_X, 2, +0.25f, +1.0f);
	MAP_ANALOG(ImGuiKey_GamepadRStickUp, GLFW_GAMEPAD_AXIS_RIGHT_Y, 3, -0.25f, -1.0f);
	MAP_ANALOG(ImGuiKey_GamepadRStickDown, GLFW_GAMEPAD_AXIS_RIGHT_Y, 3, +0.25f, +1.0f);
#undef MAP_BUTTON
#undef MAP_ANALOG
}

static void ImGui_ImplGlfw_UpdateMonitors()
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	bd->WantUpdateMonitors = false;

	int monitors_count = 0;
	GLFWmonitor** glfw_monitors = glfwGetMonitors(&monitors_count);
	if (monitors_count == 0) // Preserve existing monitor list if there are none. Happens on macOS sleeping (#5683)
		return;

	platform_io.Monitors.resize(0);
	for (int n = 0; n < monitors_count; n++)
	{
		ImGuiPlatformMonitor monitor;
		int x, y;
		glfwGetMonitorPos(glfw_monitors[n], &x, &y);
		const GLFWvidmode* vid_mode = glfwGetVideoMode(glfw_monitors[n]);
		if (vid_mode == nullptr)
			continue; // Failed to get Video mode (e.g. Emscripten does not support this function)
		monitor.MainPos = monitor.WorkPos = ImVec2((float)x, (float)y);
		monitor.MainSize = monitor.WorkSize = ImVec2((float)vid_mode->width, (float)vid_mode->height);
#if GLFW_HAS_MONITOR_WORK_AREA
		int w, h;
		glfwGetMonitorWorkarea(glfw_monitors[n], &x, &y, &w, &h);
		if (w > 0 && h > 0) // Workaround a small GLFW issue reporting zero on monitor changes: https://github.com/glfw/glfw/pull/1761
		{
			monitor.WorkPos = ImVec2((float)x, (float)y);
			monitor.WorkSize = ImVec2((float)w, (float)h);
		}
#endif
#if GLFW_HAS_PER_MONITOR_DPI
		// Warning: the validity of monitor DPI information on Windows depends on the application DPI awareness settings, which generally needs to be set in the manifest or at runtime.
		float x_scale, y_scale;
		glfwGetMonitorContentScale(glfw_monitors[n], &x_scale, &y_scale);
		if (x_scale == 0.0f)
			continue; // Some accessibility applications are declaring virtual monitors with a DPI of 0, see #7902.
		monitor.DpiScale = x_scale;
#endif
		monitor.PlatformHandle = (void*)glfw_monitors[n]; // [...] GLFW doc states: "guaranteed to be valid only until the monitor configuration changes"
		platform_io.Monitors.push_back(monitor);
	}
}

void ImGui_ImplGlfwpp_NewFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplGlfw_InitForXXX()?");

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	glfwGetWindowSize(bd->Window, &w, &h);
	glfwGetFramebufferSize(bd->Window, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	if (w > 0 && h > 0)
		io.DisplayFramebufferScale = ImVec2((float)display_w / (float)w, (float)display_h / (float)h);
	if (bd->WantUpdateMonitors)
		ImGui_ImplGlfw_UpdateMonitors();

	// Setup time step
	// (Accept glfwGetTime() not returning a monotonically increasing value. Seems to happens on disconnecting peripherals and probably on VMs and Emscripten, see #6491, #6189, #6114, #3644)
	double current_time = glfwGetTime();
	if (current_time <= bd->Time)
		current_time = bd->Time + 0.00001f;
	io.DeltaTime = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
	bd->Time = current_time;

	bd->MouseIgnoreButtonUp = false;
	ImGui_ImplGlfw_UpdateMouseData();
	ImGui_ImplGlfw_UpdateMouseCursor();

	// Update game controllers (if enabled and available)
	ImGui_ImplGlfw_UpdateGamepads();
}

// GLFW doesn't provide a portable sleep function
void ImGui_ImplGlfwpp_Sleep(int milliseconds)
{
#ifdef _WIN32
	::Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}

#ifdef EMSCRIPTEN_USE_EMBEDDED_GLFW3
static EM_BOOL ImGui_ImplGlfw_OnCanvasSizeChange(int event_type, const EmscriptenUiEvent* event, void* user_data)
{
	ImGui_ImplGlfw_Data* bd = (ImGui_ImplGlfw_Data*)user_data;
	double canvas_width, canvas_height;
	emscripten_get_element_css_size(bd->CanvasSelector, &canvas_width, &canvas_height);
	glfwSetWindowSize(bd->Window, (int)canvas_width, (int)canvas_height);
	return true;
}

static EM_BOOL ImGui_ImplEmscripten_FullscreenChangeCallback(int event_type, const EmscriptenFullscreenChangeEvent* event, void* user_data)
{
	ImGui_ImplGlfw_Data* bd = (ImGui_ImplGlfw_Data*)user_data;
	double canvas_width, canvas_height;
	emscripten_get_element_css_size(bd->CanvasSelector, &canvas_width, &canvas_height);
	glfwSetWindowSize(bd->Window, (int)canvas_width, (int)canvas_height);
	return true;
}

// 'canvas_selector' is a CSS selector. The event listener is applied to the first element that matches the query.
// STRING MUST PERSIST FOR THE APPLICATION DURATION. PLEASE USE A STRING LITERAL OR ENSURE POINTER WILL STAY VALID.
void ImGui_ImplGlfw_InstallEmscriptenCallbacks(GLFWwindow*, const char* canvas_selector)
{
	IM_ASSERT(canvas_selector != nullptr);
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplGlfw_InitForXXX()?");

	bd->CanvasSelector = canvas_selector;
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, bd, false, ImGui_ImplGlfw_OnCanvasSizeChange);
	emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, bd, false, ImGui_ImplEmscripten_FullscreenChangeCallback);

	// Change the size of the GLFW window according to the size of the canvas
	ImGui_ImplGlfw_OnCanvasSizeChange(EMSCRIPTEN_EVENT_RESIZE, {}, bd);

	// Register Emscripten Wheel callback to workaround issue in Emscripten GLFW Emulation (#6096)
	// We intentionally do not check 'if (install_callbacks)' here, as some users may set it to false and call GLFW callback themselves.
	// FIXME: May break chaining in case user registered their own Emscripten callback?
	emscripten_set_wheel_callback(bd->CanvasSelector, nullptr, false, ImGui_ImplEmscripten_WheelCallback);
}
#elif defined(EMSCRIPTEN_USE_PORT_CONTRIB_GLFW3)
// When using --use-port=contrib.glfw3 for the GLFW implementation, you can override the behavior of this call
// by invoking emscripten_glfw_make_canvas_resizable afterward.
// See https://github.com/pongasoft/emscripten-glfw/blob/master/docs/Usage.md#how-to-make-the-canvas-resizable-by-the-user for an explanation
void ImGui_ImplGlfw_InstallEmscriptenCallbacks(GLFWwindow* window, const char* canvas_selector)
{
	GLFWwindow* w = (GLFWwindow*)(EM_ASM_INT({ return Module.glfwGetWindow(UTF8ToString($0)); }, canvas_selector));
	IM_ASSERT(window == w); // Sanity check
	IM_UNUSED(w);
	emscripten_glfw_make_canvas_resizable(window, "window", nullptr);
}
#endif // #ifdef EMSCRIPTEN_USE_PORT_CONTRIB_GLFW3


//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

// Helper structure we store in the void* RendererUserData field of each ImGuiViewport to easily retrieve our backend data.
struct ImGui_ImplGlfw_ViewportData
{
	GLFWwindow* Window = nullptr;
	bool        WindowOwned = false;
	int         IgnoreWindowPosEventFrame = -1;
	int         IgnoreWindowSizeEventFrame = 0;
#ifdef _WIN32
	WNDPROC     PrevWndProc;
#endif

	std::vector<CallMe::Subscription> Subscriptions = {};

	ImGui_ImplGlfw_ViewportData() {}
	~ImGui_ImplGlfw_ViewportData() { IM_ASSERT(Window == nullptr); }
};

static void ImGui_ImplGlfwpp_WindowCloseCallback(glfw::Window& window)
{
	if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
		viewport->PlatformRequestClose = true;
}

// GLFW may dispatch window pos/size events after calling glfwSetWindowPos()/glfwSetWindowSize().
// However: depending on the platform the callback may be invoked at different time:
// - on Windows it appears to be called within the glfwSetWindowPos()/glfwSetWindowSize() call
// - on Linux it is queued and invoked during glfwPollEvents()
// Because the event doesn't always fire on glfwSetWindowXXX() we use a frame counter tag to only
// ignore recent glfwSetWindowXXX() calls.
static void ImGui_ImplGlfwpp_WindowPosCallback(glfw::Window& window, int, int)
{
	if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
	{
		if (ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData)
		{
			bool ignore_event = (ImGui::GetFrameCount() <= vd->IgnoreWindowPosEventFrame + 1);
			//data->IgnoreWindowPosEventFrame = -1;
			if (ignore_event)
				return;
		}
		viewport->PlatformRequestMove = true;
	}
}

static void ImGui_ImplGlfwpp_WindowSizeCallback(glfw::Window& window, int, int)
{
	if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
	{
		if (ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData)
		{
			bool ignore_event = (ImGui::GetFrameCount() <= vd->IgnoreWindowSizeEventFrame + 1);
			//data->IgnoreWindowSizeEventFrame = -1;
			if (ignore_event)
				return;
		}
		viewport->PlatformRequestResize = true;
	}
}

static void ImGui_ImplGlfw_CreateWindow(ImGuiViewport* viewport)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	ImGui_ImplGlfw_ViewportData* vd = IM_NEW(ImGui_ImplGlfw_ViewportData)();
	viewport->PlatformUserData = vd;

	// Workaround for Linux: ignore mouse up events corresponding to losing focus of the previously focused window (#7733, #3158, #7922)
#ifdef __linux__
	bd->MouseIgnoreButtonUpWaitForFocusLoss = true;
#endif

	// GLFW 3.2 unfortunately always set focus on glfwCreateWindow() if GLFW_VISIBLE is set, regardless of GLFW_FOCUSED
	// With GLFW 3.3, the hint GLFW_FOCUS_ON_SHOW fixes this problem
	glfwWindowHint(GLFW_VISIBLE, false);
	glfwWindowHint(GLFW_FOCUSED, false);
#if GLFW_HAS_FOCUS_ON_SHOW
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, false);
#endif
	glfwWindowHint(GLFW_DECORATED, (viewport->Flags & ImGuiViewportFlags_NoDecoration) ? false : true);
#if GLFW_HAS_WINDOW_TOPMOST
	glfwWindowHint(GLFW_FLOATING, (viewport->Flags & ImGuiViewportFlags_TopMost) ? true : false);
#endif
	glfw::Window* share_window = (bd->ClientApi == GlfwClientApi_OpenGL) ? &glfw::Window::getWrapperFromHandle(bd->Window) : nullptr;
	glfw::Window tmp = glfw::Window((int)viewport->Size.x, (int)viewport->Size.y, "No Title Yet", nullptr, share_window);
	vd->Window = tmp;
	vd->WindowOwned = true;
	viewport->PlatformHandle = (void*)vd->Window;
#ifdef _WIN32
	viewport->PlatformHandleRaw = glfwGetWin32Window(vd->Window);
#elif defined(__APPLE__)
	viewport->PlatformHandleRaw = (void*)glfwGetCocoaWindow(vd->Window);
#endif
	tmp.setPos((int)viewport->Pos.x, (int)viewport->Pos.y);

	// Install GLFW callbacks for secondary viewports
	vd->Subscriptions.reserve(10);
	vd->Subscriptions.emplace_back(tmp.focusEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_WindowFocusCallback>()));
	vd->Subscriptions.emplace_back(tmp.cursorEnterEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_CursorEnterCallback>()));
	vd->Subscriptions.emplace_back(tmp.cursorPosEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_CursorPosCallback>()));
	vd->Subscriptions.emplace_back(tmp.mouseButtonEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_MouseButtonCallback>()));
	vd->Subscriptions.emplace_back(tmp.scrollEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_ScrollCallback>()));
	vd->Subscriptions.emplace_back(tmp.keyEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_KeyCallback>()));
	vd->Subscriptions.emplace_back(tmp.charEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_CharCallback>()));
	vd->Subscriptions.emplace_back(tmp.closeEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_WindowCloseCallback>()));
	vd->Subscriptions.emplace_back(tmp.posEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_WindowPosCallback>()));
	vd->Subscriptions.emplace_back(tmp.sizeEvent.subscribe(CallMe::fromFunction<&ImGui_ImplGlfwpp_WindowSizeCallback>()));
	if (bd->ClientApi == GlfwClientApi_OpenGL)
	{
		glfwMakeContextCurrent(vd->Window);
		glfwSwapInterval(0);
	}
}

static void ImGui_ImplGlfw_DestroyWindow(ImGuiViewport* viewport)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	if (ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData)
	{
		if (vd->WindowOwned)
		{
#if !GLFW_HAS_MOUSE_PASSTHROUGH && GLFW_HAS_WINDOW_HOVERED && defined(_WIN32)
			HWND hwnd = (HWND)viewport->PlatformHandleRaw;
			::RemovePropA(hwnd, "IMGUI_VIEWPORT");
#endif

			// Release any keys that were pressed in the window being destroyed and are still held down,
			// because we will not receive any release events after window is destroyed.
			for (int i = 0; i < IM_ARRAYSIZE(bd->KeyOwnerWindows); i++)
				if (bd->KeyOwnerWindows[i] == vd->Window)
					ImGui_ImplGlfwpp_KeyCallback(glfw::Window::getWrapperFromHandle(vd->Window), static_cast<glfw::KeyCode>(i), 0,
						glfw::KeyState::Release, static_cast<glfw::ModifierKeyBit>(0)); // Later params are only used for main viewport, on which this function is never called.

			vd->Subscriptions.clear();
			glfwDestroyWindow(vd->Window);
		}
		vd->Window = nullptr;
		IM_DELETE(vd);
	}
	viewport->PlatformUserData = viewport->PlatformHandle = nullptr;
}

static void ImGui_ImplGlfw_ShowWindow(ImGuiViewport* viewport)
{
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;

#if defined(_WIN32)
	// GLFW hack: Hide icon from task bar
	HWND hwnd = (HWND)viewport->PlatformHandleRaw;
	if (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon)
	{
		LONG ex_style = ::GetWindowLong(hwnd, GWL_EXSTYLE);
		ex_style &= ~WS_EX_APPWINDOW;
		ex_style |= WS_EX_TOOLWINDOW;
		::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
	}

	// GLFW hack: install hook for WM_NCHITTEST message handler
#if !GLFW_HAS_MOUSE_PASSTHROUGH && GLFW_HAS_WINDOW_HOVERED && defined(_WIN32)
	::SetPropA(hwnd, "IMGUI_VIEWPORT", viewport);
	vd->PrevWndProc = (WNDPROC)::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
	::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)ImGui_ImplGlfw_WndProc);
#endif

#if !GLFW_HAS_FOCUS_ON_SHOW
	// GLFW hack: GLFW 3.2 has a bug where glfwShowWindow() also activates/focus the window.
	// The fix was pushed to GLFW repository on 2018/01/09 and should be included in GLFW 3.3 via a GLFW_FOCUS_ON_SHOW window attribute.
	// See https://github.com/glfw/glfw/issues/1189
	// FIXME-VIEWPORT: Implement same work-around for Linux/OSX in the meanwhile.
	if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
	{
		::ShowWindow(hwnd, SW_SHOWNA);
		return;
	}
#endif
#endif

	glfwShowWindow(vd->Window);
}

static ImVec2 ImGui_ImplGlfw_GetWindowPos(ImGuiViewport* viewport)
{
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	int x = 0, y = 0;
	glfwGetWindowPos(vd->Window, &x, &y);
	return ImVec2((float)x, (float)y);
}

static void ImGui_ImplGlfw_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	vd->IgnoreWindowPosEventFrame = ImGui::GetFrameCount();
	glfwSetWindowPos(vd->Window, (int)pos.x, (int)pos.y);
}

static ImVec2 ImGui_ImplGlfw_GetWindowSize(ImGuiViewport* viewport)
{
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	int w = 0, h = 0;
	glfwGetWindowSize(vd->Window, &w, &h);
	return ImVec2((float)w, (float)h);
}

static void ImGui_ImplGlfw_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
#if __APPLE__ && !GLFW_HAS_OSX_WINDOW_POS_FIX
	// Native OS windows are positioned from the bottom-left corner on macOS, whereas on other platforms they are
	// positioned from the upper-left corner. GLFW makes an effort to convert macOS style coordinates, however it
	// doesn't handle it when changing size. We are manually moving the window in order for changes of size to be based
	// on the upper-left corner.
	int x, y, width, height;
	glfwGetWindowPos(vd->Window, &x, &y);
	glfwGetWindowSize(vd->Window, &width, &height);
	glfwSetWindowPos(vd->Window, x, y - height + size.y);
#endif
	vd->IgnoreWindowSizeEventFrame = ImGui::GetFrameCount();
	glfwSetWindowSize(vd->Window, (int)size.x, (int)size.y);
}

static void ImGui_ImplGlfw_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	glfwSetWindowTitle(vd->Window, title);
}

static void ImGui_ImplGlfw_SetWindowFocus(ImGuiViewport* viewport)
{
#if GLFW_HAS_FOCUS_WINDOW
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	glfwFocusWindow(vd->Window);
#else
	// FIXME: What are the effect of not having this function? At the moment imgui doesn't actually call SetWindowFocus - we set that up ahead, will answer that question later.
	(void)viewport;
#endif
}

static bool ImGui_ImplGlfw_GetWindowFocus(ImGuiViewport* viewport)
{
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	return glfwGetWindowAttrib(vd->Window, GLFW_FOCUSED) != 0;
}

static bool ImGui_ImplGlfw_GetWindowMinimized(ImGuiViewport* viewport)
{
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	return glfwGetWindowAttrib(vd->Window, GLFW_ICONIFIED) != 0;
}

#if GLFW_HAS_WINDOW_ALPHA
static void ImGui_ImplGlfw_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	glfwSetWindowOpacity(vd->Window, alpha);
}
#endif

static void ImGui_ImplGlfw_RenderWindow(ImGuiViewport* viewport, void*)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	if (bd->ClientApi == GlfwClientApi_OpenGL)
		glfwMakeContextCurrent(vd->Window);
}

static void ImGui_ImplGlfw_SwapBuffers(ImGuiViewport* viewport, void*)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	if (bd->ClientApi == GlfwClientApi_OpenGL)
	{
		glfwMakeContextCurrent(vd->Window);
		glfwSwapBuffers(vd->Window);
	}
}

//--------------------------------------------------------------------------------------------------------
// Vulkan support (the Vulkan renderer needs to call a platform-side support function to create the surface)
//--------------------------------------------------------------------------------------------------------

// Avoid including <vulkan.h> so we can build without it
#if GLFW_HAS_VULKAN
#ifndef VULKAN_H_
#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;
#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;
#else
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#endif
VK_DEFINE_HANDLE(VkInstance)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSurfaceKHR)
struct VkAllocationCallbacks;
enum VkResult { VK_RESULT_MAX_ENUM = 0x7FFFFFFF };
#endif // VULKAN_H_
extern "C" { extern GLFWAPI VkResult glfwCreateWindowSurface(VkInstance instance, GLFWwindow* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface); }
static int ImGui_ImplGlfw_CreateVkSurface(ImGuiViewport* viewport, ImU64 vk_instance, const void* vk_allocator, ImU64* out_vk_surface)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData;
	IM_UNUSED(bd);
	IM_ASSERT(bd->ClientApi == GlfwClientApi_Vulkan);
	VkResult err = glfwCreateWindowSurface((VkInstance)vk_instance, vd->Window, (const VkAllocationCallbacks*)vk_allocator, (VkSurfaceKHR*)out_vk_surface);
	return (int)err;
}
#endif // GLFW_HAS_VULKAN

static void ImGui_ImplGlfw_InitMultiViewportSupport()
{
	// Register platform interface (will be coupled with a renderer interface)
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Platform_CreateWindow = ImGui_ImplGlfw_CreateWindow;
	platform_io.Platform_DestroyWindow = ImGui_ImplGlfw_DestroyWindow;
	platform_io.Platform_ShowWindow = ImGui_ImplGlfw_ShowWindow;
	platform_io.Platform_SetWindowPos = ImGui_ImplGlfw_SetWindowPos;
	platform_io.Platform_GetWindowPos = ImGui_ImplGlfw_GetWindowPos;
	platform_io.Platform_SetWindowSize = ImGui_ImplGlfw_SetWindowSize;
	platform_io.Platform_GetWindowSize = ImGui_ImplGlfw_GetWindowSize;
	platform_io.Platform_SetWindowFocus = ImGui_ImplGlfw_SetWindowFocus;
	platform_io.Platform_GetWindowFocus = ImGui_ImplGlfw_GetWindowFocus;
	platform_io.Platform_GetWindowMinimized = ImGui_ImplGlfw_GetWindowMinimized;
	platform_io.Platform_SetWindowTitle = ImGui_ImplGlfw_SetWindowTitle;
	platform_io.Platform_RenderWindow = ImGui_ImplGlfw_RenderWindow;
	platform_io.Platform_SwapBuffers = ImGui_ImplGlfw_SwapBuffers;
#if GLFW_HAS_WINDOW_ALPHA
	platform_io.Platform_SetWindowAlpha = ImGui_ImplGlfw_SetWindowAlpha;
#endif
#if GLFW_HAS_VULKAN
	platform_io.Platform_CreateVkSurface = ImGui_ImplGlfw_CreateVkSurface;
#endif

	// Register main window handle (which is owned by the main application, not by us)
	// This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for main and secondary viewports.
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui_ImplGlfw_ViewportData* vd = IM_NEW(ImGui_ImplGlfw_ViewportData)();
	vd->Window = bd->Window;
	vd->WindowOwned = false;
	main_viewport->PlatformUserData = vd;
	main_viewport->PlatformHandle = (void*)bd->Window;
}

static void ImGui_ImplGlfw_ShutdownMultiViewportSupport()
{
	ImGui::DestroyPlatformWindows();
}

//-----------------------------------------------------------------------------

// WndProc hook (declared here because we will need access to ImGui_ImplGlfw_ViewportData)
#ifdef _WIN32
static ImGuiMouseSource GetMouseSourceFromMessageExtraInfo()
{
	LPARAM extra_info = ::GetMessageExtraInfo();
	if ((extra_info & 0xFFFFFF80) == 0xFF515700)
		return ImGuiMouseSource_Pen;
	if ((extra_info & 0xFFFFFF80) == 0xFF515780)
		return ImGuiMouseSource_TouchScreen;
	return ImGuiMouseSource_Mouse;
}
static LRESULT CALLBACK ImGui_ImplGlfw_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
	WNDPROC prev_wndproc = bd->PrevWndProc;
	ImGuiViewport* viewport = (ImGuiViewport*)::GetPropA(hWnd, "IMGUI_VIEWPORT");
	if (viewport != NULL)
		if (ImGui_ImplGlfw_ViewportData* vd = (ImGui_ImplGlfw_ViewportData*)viewport->PlatformUserData)
			prev_wndproc = vd->PrevWndProc;

	switch (msg)
	{
		// GLFW doesn't allow to distinguish Mouse vs TouchScreen vs Pen.
		// Add support for Win32 (based on imgui_impl_win32), because we rely on _TouchScreen info to trickle inputs differently.
	case WM_MOUSEMOVE: case WM_NCMOUSEMOVE:
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK: case WM_LBUTTONUP:
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK: case WM_RBUTTONUP:
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK: case WM_MBUTTONUP:
	case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK: case WM_XBUTTONUP:
		ImGui::GetIO().AddMouseSourceEvent(GetMouseSourceFromMessageExtraInfo());
		break;

		// We have submitted https://github.com/glfw/glfw/pull/1568 to allow GLFW to support "transparent inputs".
		// In the meanwhile we implement custom per-platform workarounds here (FIXME-VIEWPORT: Implement same work-around for Linux/OSX!)
#if !GLFW_HAS_MOUSE_PASSTHROUGH && GLFW_HAS_WINDOW_HOVERED
	case WM_NCHITTEST:
	{
		// Let mouse pass-through the window. This will allow the backend to call io.AddMouseViewportEvent() properly (which is OPTIONAL).
		// The ImGuiViewportFlags_NoInputs flag is set while dragging a viewport, as want to detect the window behind the one we are dragging.
		// If you cannot easily access those viewport flags from your windowing/event code: you may manually synchronize its state e.g. in
		// your main loop after calling UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
		if (viewport && (viewport->Flags & ImGuiViewportFlags_NoInputs))
			return HTTRANSPARENT;
		break;
	}
#endif
	}
	return ::CallWindowProcW(prev_wndproc, hWnd, msg, wParam, lParam);
}
#endif // #ifdef _WIN32

//-----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif // #ifndef IMGUI_DISABLE

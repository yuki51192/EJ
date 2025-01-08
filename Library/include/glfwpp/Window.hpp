#ifndef GLFWWRAPPER_WINDOW_HPP
#define GLFWWRAPPER_WINDOW_HPP

#include <optional>
#include <span>

#include <glm/vec2.hpp>

#include <CallMe/CallMe.Event.h>

#include <GLFW/glfw3.h>
#include "Error.hpp"
#include "Enums.hpp"
#include "Cursor.hpp"
#include "Version.hpp"
#include "Monitor.hpp"

namespace glfw
{

    class Window
    {
    public:
        explicit Window() noexcept = default;
        explicit Window(std::nullptr_t) noexcept :
            Window{}
        {}

        explicit Window(GLFWwindow* handle_) :
            _handle{ handle_ }
        {
            if (_handle)
            {
                _setPointerForHandle(_handle, this);

                glfwSetWindowPosCallback(_handle, _posCallback);
                glfwSetWindowSizeCallback(_handle, _sizeCallback);
                glfwSetWindowCloseCallback(_handle, _closeCallback);
                glfwSetWindowRefreshCallback(_handle, _refreshCallback);
                glfwSetWindowFocusCallback(_handle, _focusCallback);
                glfwSetWindowIconifyCallback(_handle, _iconifyCallback);
                glfwSetWindowMaximizeCallback(_handle, _maximizeCallback);
                glfwSetFramebufferSizeCallback(_handle, _framebufferSizeCallback);
                glfwSetWindowContentScaleCallback(_handle, _contentScaleCallback);

                glfwSetKeyCallback(_handle, _keyCallback);
                glfwSetCharCallback(_handle, _charCallback);
                glfwSetMouseButtonCallback(_handle, _mouseButtonCallback);
                glfwSetCursorPosCallback(_handle, _cursorPosCallback);
                glfwSetCursorEnterCallback(_handle, _cursorEnterCallback);
                glfwSetScrollCallback(_handle, _scrollCallback);
                glfwSetDropCallback(_handle, _dropCallback);
            }
        }

        Window(int width_,
            int height_,
            const char* title_,
            const Monitor* monitor_ = nullptr,
            const Window* share_ = nullptr) :
            Window{ glfwCreateWindow(
                    width_,
                    height_,
                    title_,
                    monitor_ ? static_cast<GLFWmonitor*>(*monitor_) : nullptr,
                    share_ ? static_cast<GLFWwindow*>(share_->_handle) : nullptr) }
        {}

        Window(const Window&) = delete;
        Window(Window&& obj_) noexcept :
            _handle{ std::move(obj_._handle) },
			_userPtr{ std::move(obj_._userPtr) }
        {
            if (_handle)
                _setPointerForHandle(_handle, this);
        }

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&& obj_) noexcept {
            glfwDestroyWindow(_handle);
            _handle = std::move(obj_._handle);
			_userPtr = std::move(obj_._userPtr);
            if (_handle)
                _setPointerForHandle(_handle, this);

            return *this;
        }

        virtual ~Window() noexcept {
            glfwDestroyWindow(static_cast<GLFWwindow*>(*this));
        }

        operator GLFWwindow* () const
        {
            return _handle;
        }

        explicit operator bool() const = delete;

		[[nodiscard]] bool isValid() const
		{
			return _handle != nullptr;
		}

        [[nodiscard]] bool shouldClose() const
        {
            return glfwWindowShouldClose(_handle);
        }

        void setShouldClose(bool value_)
        {
            glfwSetWindowShouldClose(_handle, value_);
        }

        void setTitle(const char* title_)
        {
            glfwSetWindowTitle(_handle, title_);
        }

        void setIcon(::std::span<Image> iconCandidates_)
        {
            glfwSetWindowIcon(_handle, static_cast<int>(iconCandidates_.size()), iconCandidates_.data());
        }

        [[nodiscard]] glm::ivec2 getPos() const
        {
            int xPos, yPos;
            glfwGetWindowPos(_handle, &xPos, &yPos);
            return glm::ivec2{ xPos, yPos };
        }

        void setPos(int xPos_, int yPos_)
        {
            glfwSetWindowPos(_handle, xPos_, yPos_);
        }

        [[nodiscard]] glm::ivec2 getSize() const
        {
            int width, height;
            glfwGetWindowSize(_handle, &width, &height);
            return glm::ivec2{ width, height };
        }

        void setSizeLimits(int minWidth_, int minHeight_, int maxWidth_, int maxHeight_)
        {
            glfwSetWindowSizeLimits(_handle, minWidth_, minHeight_, maxWidth_, maxHeight_);
        }

        void setAspectRatio(int numerator_, int denominator_)
        {
            glfwSetWindowAspectRatio(_handle, numerator_, denominator_);
        }

        void setSize(int width_, int height_)
        {
            glfwSetWindowSize(_handle, width_, height_);
        }

        [[nodiscard]] glm::ivec2 getFramebufferSize() const
        {
            int width, height;
            glfwGetFramebufferSize(_handle, &width, &height);
            return glm::ivec2{ width, height };
        }

        [[nodiscard]] glm::vec2 getContentScale() const
        {
            float xScale, yScale;
            glfwGetWindowContentScale(_handle, &xScale, &yScale);
            return glm::vec2{ xScale, yScale };
        }

        [[nodiscard]] float getOpacity() const
        {
            return glfwGetWindowOpacity(_handle);
        }

        void setOpacity(float opacity_)
        {
            glfwSetWindowOpacity(_handle, opacity_);
        }

        void iconify()
        {
            glfwIconifyWindow(_handle);
        }

        void restore()
        {
            glfwRestoreWindow(_handle);
        }

        void maximize()
        {
            glfwMaximizeWindow(_handle);
        }

        void show()
        {
            glfwShowWindow(_handle);
        }

        void hide()
        {
            glfwHideWindow(_handle);
        }

        void focus()
        {
            glfwFocusWindow(_handle);
        }

        void requestAttention()
        {
            glfwRequestWindowAttention(_handle);
        }

        [[nodiscard]] Monitor getMonitor() const
        {
            return Monitor{ glfwGetWindowMonitor(_handle) };
        }

        void setMonitor(Monitor monitor_, int xPos_, int yPos_, int width_, int height_, int refreshRate_)
        {
            glfwSetWindowMonitor(_handle, static_cast<GLFWmonitor*>(monitor_), xPos_, yPos_, width_, height_, refreshRate_);
        }

        [[nodiscard]] bool getAttribFocused() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_FOCUSED);
        }

        [[nodiscard]] bool getAttribIconified() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_ICONIFIED);
        }

        [[nodiscard]] bool getAttribMaximized() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_MAXIMIZED);
        }

        [[nodiscard]] bool getAttribHovered() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_HOVERED);
        }

        [[nodiscard]] bool getAttribVisible() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_VISIBLE);
        }

        [[nodiscard]] bool getAttribResizable() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_RESIZABLE);
        }

        [[nodiscard]] bool getAttribDecorated() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_DECORATED);
        }

        [[nodiscard]] bool getAttribAutoIconify() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_AUTO_ICONIFY);
        }

        [[nodiscard]] bool getAttribFloating() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_FLOATING);
        }

        [[nodiscard]] bool getAttribTransparentFramebuffer() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_TRANSPARENT_FRAMEBUFFER);
        }
        [[nodiscard]] bool getAttribFocusOnShow() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_FOCUS_ON_SHOW);
        }

        [[nodiscard]] ClientApi getAttribClientApi() const
        {
            return static_cast<ClientApi>(glfwGetWindowAttrib(_handle, GLFW_CLIENT_API));
        }

        [[nodiscard]] ContextCreationApi getAttribContextCreationApi() const
        {
            return static_cast<ContextCreationApi>(glfwGetWindowAttrib(_handle, GLFW_CONTEXT_CREATION_API));
        }

        [[nodiscard]] Version getAttribContextVersion() const
        {
            return {
                    glfwGetWindowAttrib(_handle, GLFW_CONTEXT_VERSION_MAJOR),
                    glfwGetWindowAttrib(_handle, GLFW_CONTEXT_VERSION_MINOR),
                    glfwGetWindowAttrib(_handle, GLFW_CONTEXT_REVISION) };
        }

        [[nodiscard]] bool getAttribOpenGlForwardCompat() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_OPENGL_FORWARD_COMPAT);
        }

        [[nodiscard]] bool getAttribOpenGlDebugContext() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_OPENGL_DEBUG_CONTEXT);
        }

        [[nodiscard]] OpenGlProfile getAttribOpenGlProfile() const
        {
            return static_cast<OpenGlProfile>(glfwGetWindowAttrib(_handle, GLFW_OPENGL_PROFILE));
        }

        [[nodiscard]] ContextReleaseBehavior getAttribContextReleaseBehavior() const
        {
            return static_cast<ContextReleaseBehavior>(glfwGetWindowAttrib(_handle, GLFW_CONTEXT_RELEASE_BEHAVIOR));
        }

        [[nodiscard]] bool getAttribContextNoError() const
        {
            return glfwGetWindowAttrib(_handle, GLFW_CONTEXT_NO_ERROR);
        }

        [[nodiscard]] ContextRobustness getAttribContextRobustness() const
        {
            return static_cast<ContextRobustness>(glfwGetWindowAttrib(_handle, GLFW_CONTEXT_ROBUSTNESS));
        }

        void setAttribDecorated(bool value_)
        {
            glfwSetWindowAttrib(_handle, GLFW_DECORATED, value_);
        }

        void setAttribResizable(bool value_)
        {
            glfwSetWindowAttrib(_handle, GLFW_RESIZABLE, value_);
        }

        void setAttribFloating(bool value_)
        {
            glfwSetWindowAttrib(_handle, GLFW_FLOATING, value_);
        }

        void setAttribAutoIconify(bool value_)
        {
            glfwSetWindowAttrib(_handle, GLFW_AUTO_ICONIFY, value_);
        }

        void setAttribFocusOnShow(bool value_)
        {
            glfwSetWindowAttrib(_handle, GLFW_FOCUS_ON_SHOW, value_);
        }

        void swapBuffers()
        {
            glfwSwapBuffers(_handle);
        }

        [[nodiscard]] CursorMode getInputModeCursor() const
        {
            return static_cast<CursorMode>(glfwGetInputMode(_handle, GLFW_CURSOR));
        }
        [[nodiscard]] bool getInputModeStickyKeys() const
        {
            return glfwGetInputMode(_handle, GLFW_STICKY_KEYS);
        }
        [[nodiscard]] bool getInputModeStickyMouseButtons() const
        {
            return glfwGetInputMode(_handle, GLFW_STICKY_MOUSE_BUTTONS);
        }
        [[nodiscard]] bool getInputModeLockKeyMods() const
        {
            return glfwGetInputMode(_handle, GLFW_LOCK_KEY_MODS);
        }
        [[nodiscard]] bool getInputModeRawMouseMotion() const
        {
            return glfwGetInputMode(_handle, GLFW_RAW_MOUSE_MOTION);
        }

        void setInputModeCursor(CursorMode mode_)
        {
            return glfwSetInputMode(_handle, GLFW_CURSOR, static_cast<int>(mode_));
        }
        void setInputModeStickyKeys(bool value_)
        {
            return glfwSetInputMode(_handle, GLFW_STICKY_KEYS, value_);
        }
        void setInputModeStickyMouseButtons(bool value_)
        {
            return glfwSetInputMode(_handle, GLFW_STICKY_MOUSE_BUTTONS, value_);
        }
        void setInputModeLockKeyMods(bool value_)
        {
            return glfwSetInputMode(_handle, GLFW_LOCK_KEY_MODS, value_);
        }
        void setInputModeRawMouseMotion(bool value_)
        {
            return glfwSetInputMode(_handle, GLFW_RAW_MOUSE_MOTION, value_);
        }

        [[nodiscard]] bool getKey(KeyCode key_) const
        {
            return glfwGetKey(_handle, static_cast<int>(key_)) != GLFW_RELEASE;
        }
        [[nodiscard]] bool getMouseButton(MouseButton button_) const
        {
            return glfwGetMouseButton(_handle, static_cast<int>(button_)) != GLFW_RELEASE;
        }

        [[nodiscard]] glm::dvec2 getCursorPos() const
        {
            double xPos, yPos;
            glfwGetCursorPos(_handle, &xPos, &yPos);
            return glm::dvec2{ xPos, yPos };
        }
        void setCursorPos(double xPos_, double yPos_)
        {
            glfwSetCursorPos(_handle, xPos_, yPos_);
        }

        void setCursor(const Cursor& cursor_)
        {
            glfwSetCursor(_handle, static_cast<GLFWcursor*>(cursor_));
        }

		void makeContextCurrent()
		{
			glfwMakeContextCurrent(_handle);
		}

        void* getUserPtr() const {
            return _userPtr;
        }
        void setUserPtr(void* ptr_) {
			_userPtr = ptr_;
		}

#if defined(VK_VERSION_1_0)
        [[nodiscard]] VkResult createSurface(
            VkInstance instance,
            const VkAllocationCallbacks* allocator,
            VkSurfaceKHR* surface)
        {
            return glfwCreateWindowSurface(instance, _handle, allocator, surface);
        }
#endif  // VK_VERSION_1_0
#ifdef VULKAN_HPP
        [[nodiscard]] vk::SurfaceKHR createSurface(
            const vk::Instance& instance,
            const std::optional<vk::AllocationCallbacks>& allocator = std::nullopt)
        {
            VkSurfaceKHR surface;
            VkResult result;
            if (allocator)
            {
                VkAllocationCallbacks allocator_tmp = *allocator;
                result = createSurface(static_cast<VkInstance>(instance), &allocator_tmp, &surface);
            }
            else
            {
                result = createSurface(static_cast<VkInstance>(instance), nullptr, &surface);
            }

            if (result < 0)
            {
                throw Error("Could not create window surface");
            }
            return static_cast<vk::SurfaceKHR>(surface);
        }
#endif  // VULKAN_HPP

    public:
        CallMe::Event<void(Window&, int x, int y)> posEvent;
        CallMe::Event<void(Window&, int w, int h)> sizeEvent;
        CallMe::Event<void(Window&)> closeEvent;
        CallMe::Event<void(Window&)> refreshEvent;
        CallMe::Event<void(Window&, bool)> focusEvent;
        CallMe::Event<void(Window&, bool)> iconifyEvent;
        CallMe::Event<void(Window&, bool)> maximizeEvent;
        CallMe::Event<void(Window&, int w, int h)> framebufferSizeEvent;
        CallMe::Event<void(Window&, float x, float y)> contentScaleEvent;

        CallMe::Event<void(Window&, KeyCode key, int scancode, KeyState action, ModifierKeyBit mods)> keyEvent;
        CallMe::Event<void(Window&, unsigned int)> charEvent;
        CallMe::Event<void(Window&, MouseButton button, MouseButtonState action, ModifierKeyBit mods)> mouseButtonEvent;
        CallMe::Event<void(Window&, double x, double y)> cursorPosEvent;
        CallMe::Event<void(Window&, bool)> cursorEnterEvent;
        CallMe::Event<void(Window&, double x, double y)> scrollEvent;
        CallMe::Event<void(Window&, std::span<const char*>)> dropEvent;

    private:
        GLFWwindow* _handle = nullptr;
        void* _userPtr = nullptr;

    private:
        static void _posCallback(GLFWwindow* window_, int xPos_, int yPos_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.posEvent.raise(wrapper, xPos_, yPos_);
        }
        static void _sizeCallback(GLFWwindow* window_, int width_, int height_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.sizeEvent.raise(wrapper, width_, height_);
        }
        static void _closeCallback(GLFWwindow* window_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.closeEvent.raise(wrapper);
        }
        static void _refreshCallback(GLFWwindow* window_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.refreshEvent.raise(wrapper);
        }
        static void _focusCallback(GLFWwindow* window_, int value_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.focusEvent.raise(wrapper, value_);
        }
        static void _iconifyCallback(GLFWwindow* window_, int value_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.iconifyEvent.raise(wrapper, value_);
        }
        static void _maximizeCallback(GLFWwindow* window_, int value_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.maximizeEvent.raise(wrapper, value_);
        }
        static void _framebufferSizeCallback(GLFWwindow* window_, int width_, int height_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.framebufferSizeEvent.raise(wrapper, width_, height_);
        }
        static void _contentScaleCallback(GLFWwindow* window_, float xScale_, float yScale_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.contentScaleEvent.raise(wrapper, xScale_, yScale_);
        }


        static void _keyCallback(GLFWwindow* window_, int key_, int scanCode_, int state_, int mods_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.keyEvent(wrapper, static_cast<KeyCode>(key_), scanCode_, static_cast<KeyState>(state_), static_cast<ModifierKeyBit>(mods_));
        }
        static void _charCallback(GLFWwindow* window_, unsigned int codePoint_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.charEvent(wrapper, codePoint_);
        }
        static void _mouseButtonCallback(GLFWwindow* window_, int button_, int state_, int mods_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.mouseButtonEvent(wrapper, static_cast<MouseButton>(button_), static_cast<MouseButtonState>(state_), static_cast<ModifierKeyBit>(mods_));
        }
        static void _cursorPosCallback(GLFWwindow* window_, double xPos_, double yPos_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.cursorPosEvent(wrapper, xPos_, yPos_);
        }
        static void _cursorEnterCallback(GLFWwindow* window_, int value_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.cursorEnterEvent(wrapper, static_cast<bool>(value_));
        }
        static void _scrollCallback(GLFWwindow* window_, double xOffset_, double yOffset_)
        {
            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.scrollEvent(wrapper, xOffset_, yOffset_);
        }
        static void _dropCallback(GLFWwindow* window_, int count_, const char** pPaths_)
        {
            std::vector<const char*> paths;
            paths.reserve(count_);

            for (int i = 0; i < count_; ++i)
            {
                paths.emplace_back(pPaths_[i]);
            }

            Window& wrapper = getWrapperFromHandle(window_);
            wrapper.dropEvent(wrapper, std::span<const char*>{ pPaths_, static_cast<std::size_t>(count_) });
        }

    // static
    public:
        static Window& getWrapperFromHandle(GLFWwindow* handle_)
        {
            return *static_cast<Window*>(glfwGetWindowUserPointer(handle_));
        }

        static void makeContextCurrent(const Window& window_)
        {
            glfwMakeContextCurrent(window_._handle);
        }
        [[nodiscard]] static Window& getCurrentContext()
        {
            return getWrapperFromHandle(glfwGetCurrentContext());
        }

		static void swapInterval(int interval_)
		{
			glfwSwapInterval(interval_);
		}

    private:
        static void _setPointerForHandle(GLFWwindow* handle_, Window* ptr_)
        {
            glfwSetWindowUserPointer(handle_, ptr_);
        }

    };

}  // namespace glfw

#endif  //GLFWWRAPPER_WINDOW_HPP
#ifndef GLFWWRAPPER_GLFW_HPP
#define GLFWWRAPPER_GLFW_HPP

#include <cassert>

#include <GLFW/glfw3.h>
#include "Error.hpp"
#include "Enums.hpp"
#include "Window.hpp"
#include "Version.hpp"
#include "Monitor.hpp"
#include "Joystick.hpp"
#include "WindowHints.hpp"

namespace glfw
{
    namespace impl
    {
        inline void errorCallback(int errorCode_, const char* what_)
        {
            // Error handling philosophy as per http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0709r4.pdf (section 1.1)

            // Application programmer errors. See the GLFW docs and fix the code.
            assert(errorCode_ != GLFW_NOT_INITIALIZED);
            assert(errorCode_ != GLFW_NO_CURRENT_CONTEXT);
            assert(errorCode_ != GLFW_NO_WINDOW_CONTEXT);
            assert(errorCode_ != GLFW_INVALID_VALUE);

            // These errors should never occur
            assert(errorCode_ != GLFW_NO_ERROR);
            assert(errorCode_ != GLFW_INVALID_ENUM);

            // Allocation failure must be treated separately
            if (errorCode_ == GLFW_OUT_OF_MEMORY)
            {
                throw std::bad_alloc();
            }

            switch (errorCode_)
            {
            case GLFW_API_UNAVAILABLE:
                throw APIUnavailableError(what_);
            case GLFW_VERSION_UNAVAILABLE:
                throw VersionUnavailableError(what_);
            case GLFW_PLATFORM_ERROR:
                throw PlatformError(what_);
            case GLFW_FORMAT_UNAVAILABLE:
                throw FormatUnavailableError(what_);
            default:
                // There should be no other error possible
                assert(false);
            }
        }

        inline void monitorCallback(GLFWmonitor* monitor_, int eventType_)
        {
			Monitor::monitorEvent.raise(Monitor{ monitor_ }, Monitor::EventType{ eventType_ });
        }

        inline void joystickCallback(int jid_, int eventType_)
        {
            Joystick::Joystick::joystickEvent.raise(Joystick{ static_cast<Joystick::EnumType>(jid_) }, Joystick::EventType{ eventType_ });
        }
    } // namespace impl

    struct InitHints
    {
        bool joystickHatButtons = GLFW_FALSE;
        bool cocoaChdirResources = GLFW_TRUE;
        bool cocoaMenubar = GLFW_TRUE;
        enum class WaylandLibdecorOption : bool {
            PREFER,
            DISABLE
        } waylandLibdecor = WaylandLibdecorOption::PREFER;

        void apply() const
        {
            glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, joystickHatButtons);
            glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, cocoaChdirResources);
            glfwInitHint(GLFW_COCOA_MENUBAR, cocoaMenubar);
            glfwInitHint(GLFW_WAYLAND_LIBDECOR, waylandLibdecor == WaylandLibdecorOption::PREFER ? GLFW_WAYLAND_PREFER_LIBDECOR : GLFW_WAYLAND_DISABLE_LIBDECOR);
        }
    };

    struct GlfwLifeHandler
    {
    private:
        GlfwLifeHandler() = default;

    public:
        ~GlfwLifeHandler()
        {
            glfwTerminate();
        }

        [[nodiscard]] friend GlfwLifeHandler init()
        {
            glfwSetErrorCallback(impl::errorCallback);

			glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, false); // no compatitable joystick with hat buttons
            if (!glfwInit())
            {
                throw glfw::Error("Could not initialize GLFW");
            }

            glfwSetMonitorCallback(impl::monitorCallback);
            glfwSetJoystickCallback(impl::joystickCallback);

            return GlfwLifeHandler{};
        }
    };

    [[nodiscard]] inline GlfwLifeHandler init();

    [[nodiscard]] inline bool rawMouseMotionSupported()
    {
        return glfwRawMouseMotionSupported();
    }

    inline void setClipboardString(const char* content_)
    {
        glfwSetClipboardString(nullptr, content_);
    }

    [[nodiscard]] inline const char* getClipboardString()
    {
        return glfwGetClipboardString(nullptr);
    }

    [[nodiscard]] inline bool extensionSupported(const char* extensionName_)
    {
        return glfwExtensionSupported(extensionName_);
    }

    using GlProc = GLFWglproc;
    [[nodiscard]] inline GlProc getProcAddress(const char* procName_)
    {
        return glfwGetProcAddress(procName_);
    }

    [[nodiscard]] inline bool vulkanSupported()
    {
        return glfwVulkanSupported();
    }

    [[nodiscard]] inline std::span<const char*> getRequiredInstanceExtensions()
    {
        unsigned count;
        auto pExtensionNames = glfwGetRequiredInstanceExtensions(&count);
        return std::span<const char*>{ pExtensionNames, count };
    }
    using VkProc = GLFWvkproc;
#if defined(VK_VERSION_1_0)
    [[nodiscard]] inline VkProc getInstanceProcAddress(VkInstance instance, const char* procName)
    {
        return glfwGetInstanceProcAddress(instance, procName);
    }

    [[nodiscard]] inline bool getPhysicalDevicePresentationSupport(
        VkInstance instance,
        VkPhysicalDevice device,
        uint32_t queueFamily)
    {
        return glfwGetPhysicalDevicePresentationSupport(instance, device, queueFamily);
    }
#endif // VK_VERSION_1_0

#ifdef VULKAN_HPP
    [[nodiscard]] inline VkProc getInstanceProcAddress(const vk::Instance& instance, const char* procName)
    {
        return getInstanceProcAddress(static_cast<VkInstance>(instance), procName);
    }
    [[nodiscard]] inline bool getPhysicalDevicePresentationSupport(
        const vk::Instance& instance,
        const vk::PhysicalDevice& device,
        uint32_t queueFamily)
    {
        return getPhysicalDevicePresentationSupport(static_cast<VkInstance>(instance), static_cast<VkPhysicalDevice>(device), queueFamily);
    }
#endif // VULKAN_HPP

    [[nodiscard]] inline double getTime()
    {
        return glfwGetTime();
    }

    inline void setTime(double time_)
    {
        glfwSetTime(time_);
    }

    [[nodiscard]] inline uint64_t getTimerValue()
    {
        return glfwGetTimerValue();
    }

    [[nodiscard]] inline uint64_t getTimerFrequency()
    {
        return glfwGetTimerFrequency();
    }

    inline void pollEvents()
    {
        glfwPollEvents();
    }

    inline void waitEvents()
    {
        glfwWaitEvents();
    }
    inline void waitEvents(double timeout_)
    {
        glfwWaitEventsTimeout(timeout_);
    }

    inline void postEmptyEvent()
    {
        glfwPostEmptyEvent();
    }

} // namespace glfw

#endif  // GLFWWRAPPER_GLFW_HPP
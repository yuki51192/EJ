#ifndef GLFWWRAPPER_MONITOR_HPP
#define GLFWWRAPPER_MONITOR_HPP

#include <span>
#include <tuple>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <CallMe/CallMe.Event.h>

#include <GLFW/glfw3.h>

namespace glfw
{
    using VideoMode = GLFWvidmode;
    using GammaRamp = GLFWgammaramp;

	// this is just a helper class to deal with things you can do with GLFWmonitors, so you should't derive from it
    class Monitor final
    {
    public:
        enum class EventType : int
        {
            Connected = GLFW_CONNECTED,
            Disconnected = GLFW_DISCONNECTED
        };

    public:
        Monitor() :
            Monitor{ nullptr }
        {}

        explicit Monitor(GLFWmonitor* handle_) :
            _handle{ handle_ }
        {}

        Monitor(const Monitor&) = default;
        Monitor(Monitor&&) noexcept = default;

        Monitor& operator=(const Monitor&) = default;
        Monitor& operator=(Monitor&&) noexcept = default;

		~Monitor() = default;

        operator GLFWmonitor* () const {
            return _handle;
        }

        explicit operator bool() const = delete;

        [[nodiscard]] glm::ivec2 getPos() const
        {
            int x, y;
            glfwGetMonitorPos(_handle, &x, &y);
            return glm::ivec2{ x, y };
        }

        [[nodiscard]] glm::ivec4 getWorkArea() const
        {
            int xPos, yPos, width, height;
            glfwGetMonitorWorkarea(_handle, &xPos, &yPos, &width, &height);
            return glm::ivec4{ xPos, yPos, width, height };
        }

        [[nodiscard]] glm::ivec2 getPhysicalSize() const
        {
            int widthMillimeters, heightMillimeters;
            glfwGetMonitorPhysicalSize(_handle, &widthMillimeters, &heightMillimeters);
            return glm::ivec2{ widthMillimeters, heightMillimeters };
        }

        [[nodiscard]] glm::vec2 getContentScale() const
        {
            float xScale, yScale;
            glfwGetMonitorContentScale(_handle, &xScale, &yScale);
            return glm::vec2{ xScale, yScale };
        }

        [[nodiscard]] const char* getName() const
        {
            return glfwGetMonitorName(_handle);
        }

        void setUserPointer(void* ptr_)
        {
            glfwSetMonitorUserPointer(_handle, ptr_);
        }

        [[nodiscard]] void* getUserPointer() const
        {
            return glfwGetMonitorUserPointer(_handle);
        }

        [[nodiscard]] std::span<const VideoMode> getVideoModes() const
        {
            int count;
            auto pModes = glfwGetVideoModes(_handle, &count);
			return std::span<const VideoMode>{ pModes, static_cast<std::size_t>(count) };
        }

        [[nodiscard]] VideoMode getVideoMode() const
        {
            return *glfwGetVideoMode(_handle);
        }

        void setGamma(float gamma_)
        {
            glfwSetGamma(_handle, gamma_);
        }

        [[nodiscard]] GammaRamp getGammaRamp() const
        {
            return *glfwGetGammaRamp(_handle);
        }

        void setGammaRamp(const GammaRamp& ramp_)
        {
            glfwSetGammaRamp(_handle, &ramp_);
        }

    private:
        GLFWmonitor* _handle;

    // static
    public:
        static inline CallMe::Event<void (Monitor, EventType)> monitorEvent;

        [[nodiscard]] static std::span<GLFWmonitor*> getMonitorsRaw() {
            int count;
            auto pMonitors = glfwGetMonitors(&count);
            return std::span<GLFWmonitor*>{ pMonitors, static_cast<std::size_t>(count) };
        }
        [[nodiscard]] static std::vector<Monitor> getMonitors()
        {
            int count;
            auto pMonitors = glfwGetMonitors(&count);

            std::vector<Monitor> monitors{};
            monitors.reserve(count);
            for (int i = 0; i < count; ++i)
            {
                monitors.emplace_back(pMonitors[i]);
            }
            return monitors;
        }
        [[nodiscard]] static Monitor getPrimaryMonitor()
        {
            return Monitor{ glfwGetPrimaryMonitor() };
        }

    };

}  // namespace glfw

#endif  //GLFWWRAPPER_MONITOR_HPP
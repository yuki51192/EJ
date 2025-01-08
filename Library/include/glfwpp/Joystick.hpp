#ifndef GLFWWRAPPER_JOYSTICK_HPP
#define GLFWWRAPPER_JOYSTICK_HPP

#include <span>
#include <array>
#include <vector>
#include <bitset>

#include <CallMe/CallMe.Event.h>

#include <GLFW/glfw3.h>
#include "Helper.hpp"

namespace glfw
{
    struct GamepadState
    {
		std::bitset<15> buttons;
        std::span<float> axes;
    };
    enum class GamepadAxis
    {
        LeftX = GLFW_GAMEPAD_AXIS_LEFT_X,
        LeftY = GLFW_GAMEPAD_AXIS_LEFT_Y,
        RightX = GLFW_GAMEPAD_AXIS_RIGHT_X,
        RightY = GLFW_GAMEPAD_AXIS_RIGHT_Y,
        LeftTrigger = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
        RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
        MaxValue = GLFW_GAMEPAD_AXIS_LAST
    };
    enum class GamepadButton
    {
        A = GLFW_GAMEPAD_BUTTON_A,
        B = GLFW_GAMEPAD_BUTTON_B,
        X = GLFW_GAMEPAD_BUTTON_X,
        Y = GLFW_GAMEPAD_BUTTON_Y,
        LeftBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
        RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
        Back = GLFW_GAMEPAD_BUTTON_BACK,
        Start = GLFW_GAMEPAD_BUTTON_START,
        Guide = GLFW_GAMEPAD_BUTTON_GUIDE,
        LeftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
        RightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
        DpadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,
        DpadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
        DpadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
        DpadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
        Cross = GLFW_GAMEPAD_BUTTON_CROSS,
        Circle = GLFW_GAMEPAD_BUTTON_CIRCLE,
        Square = GLFW_GAMEPAD_BUTTON_SQUARE,
        Triangle = GLFW_GAMEPAD_BUTTON_TRIANGLE,
        MaxValue = GLFW_GAMEPAD_BUTTON_LAST
    };
    enum class JoystickHatStateBit
    {
        Centered = GLFW_HAT_CENTERED,
        Up = GLFW_HAT_UP,
        Right = GLFW_HAT_RIGHT,
        Down = GLFW_HAT_DOWN,
        Left = GLFW_HAT_LEFT,
        RightUp = GLFW_HAT_RIGHT_UP,
        RightDown = GLFW_HAT_RIGHT_DOWN,
        LeftUp = GLFW_HAT_LEFT_UP,
        LeftDown = GLFW_HAT_LEFT_DOWN
    };
    GLFWWRAPPER_ENUM_BITS_OPERATORS(JoystickHatStateBit)

    class Joystick
    {
    public:
        enum EnumType : unsigned char
        {
            Joystick1 = GLFW_JOYSTICK_1,
            Joystick2 = GLFW_JOYSTICK_2,
            Joystick3 = GLFW_JOYSTICK_3,
            Joystick4 = GLFW_JOYSTICK_4,
            Joystick5 = GLFW_JOYSTICK_5,
            Joystick6 = GLFW_JOYSTICK_6,
            Joystick7 = GLFW_JOYSTICK_7,
            Joystick8 = GLFW_JOYSTICK_8,
            Joystick9 = GLFW_JOYSTICK_9,
            Joystick10 = GLFW_JOYSTICK_10,
            Joystick11 = GLFW_JOYSTICK_11,
            Joystick12 = GLFW_JOYSTICK_12,
            Joystick13 = GLFW_JOYSTICK_13,
            Joystick14 = GLFW_JOYSTICK_14,
            Joystick15 = GLFW_JOYSTICK_15,
            Joystick16 = GLFW_JOYSTICK_16,
        };

        enum class EventType
        {
            Connected = GLFW_CONNECTED,
            Disconnected = GLFW_DISCONNECTED
        };

    public:
        Joystick() = default;
        constexpr Joystick(EnumType jid_) :
            _id{ jid_ }
        {}
        Joystick(const Joystick&) = default;
        Joystick(Joystick&&) noexcept = default;
        Joystick& operator=(const Joystick&) = default;
        Joystick& operator=(Joystick&&) noexcept = default;
        operator EnumType() const
        {
            return _id;
        }
        explicit operator bool() = delete;

        [[nodiscard]] bool isPresent() const
        {
            return glfwJoystickPresent(_id);
        }

        [[nodiscard]] std::span<const float> getAxes() const
        {
            int count;
            auto pAxes = glfwGetJoystickAxes(_id, &count);
            return std::span<const float>{ pAxes, static_cast<std::size_t>(count) };
        }

        [[nodiscard]] std::vector<bool> getButtons() const
        {
            int count;
            auto pButtons = glfwGetJoystickButtons(_id, &count);

            std::vector<bool> buttons;
            buttons.reserve(count);
            for (int i = 0; i < count; ++i)
            {
                buttons.push_back(pButtons[i] != GLFW_RELEASE);
            }

            return buttons;
        }

        [[nodiscard]] std::span<const unsigned char> getHatsRaw() const {
            int count;
            auto pHats = glfwGetJoystickHats(_id, &count);
			return std::span<const unsigned char>{ pHats, static_cast<std::size_t>(count) };
        }
        [[nodiscard]] std::vector<JoystickHatStateBit> getHats() const
        {
            int count;
            auto pHats = glfwGetJoystickHats(_id, &count);

            std::vector<JoystickHatStateBit> hats;
            hats.reserve(count);
            for (int i = 0; i < count; ++i)
            {
                hats.push_back(static_cast<JoystickHatStateBit>(pHats[i]));
            }
            return hats;
        }

        [[nodiscard]] const char* getName() const
        {
            return glfwGetJoystickName(_id);
        }

        [[nodiscard]] const char* getGuid() const
        {
            return glfwGetJoystickGUID(_id);
        }

        void setUserPointer(void* ptr_)
        {
            glfwSetJoystickUserPointer(_id, ptr_);
        }

        [[nodiscard]] void* getUserPointer() const
        {
            return glfwGetJoystickUserPointer(_id);
        }

        [[nodiscard]] bool isGamepad() const
        {
            return glfwJoystickIsGamepad(_id);
        }

        [[nodiscard]] const char* getGamepadName() const
        {
            return glfwGetGamepadName(_id);
        }

        [[nodiscard]] GamepadState getGamepadState() const
        {
            GLFWgamepadstate state;
            glfwGetGamepadState(_id, &state);

            GamepadState result{};
            for (int i = 0; i < result.buttons.size(); ++i)
            {
                result.buttons[i] = state.buttons[i] != GLFW_RELEASE;
            }
            result.axes = std::span<float>{ state.axes, state.axes + sizeof(state.axes) / sizeof(state.axes[0]) };
            return result;
        }

    private:
        EnumType _id;

    // static
    public:
        static inline CallMe::Event<void(Joystick, EventType)> joystickEvent;

        [[nodiscard]] static inline bool updateGamepadMappings(const char* string_)
        {
            return glfwUpdateGamepadMappings(string_);
        }

    };

}  // namespace glfw

#endif  //GLFWWRAPPER_JOYSTICK_HPP
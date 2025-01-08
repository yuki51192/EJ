#ifndef GLFWWRAPPER_CURSOR_HPP
#define GLFWWRAPPER_CURSOR_HPP

#include <utility>

#include <GLFW/glfw3.h>

namespace glfw
{
    using Image = GLFWimage;

    class Cursor {
    public:
        Cursor(const Cursor&) = delete;
        Cursor(Cursor&&) noexcept = default;

        Cursor& operator=(const Cursor&) = delete;
        Cursor& operator=(Cursor&& obj_) noexcept
        {
            glfwDestroyCursor(_handle);
			_handle = std::move(obj_._handle);
            return *this;
        }

        ~Cursor() noexcept
        {
            glfwDestroyCursor(static_cast<GLFWcursor*>(*this));
        }

        operator GLFWcursor* () const {
            return _handle;
        }

    protected:
        Cursor() noexcept = delete;
        explicit Cursor(GLFWcursor* handle_) :
            _handle{ handle_ }
        {}

    private:
		GLFWcursor* _handle = nullptr;

    // static
    public:
        [[nodiscard]] static Cursor createCursor(const Image& image_, int xHot_, int yHot_)
        {
            return Cursor{ glfwCreateCursor(&image_, xHot_, yHot_) };
        }
        [[nodiscard]] static Cursor createStandardCursorArrow()
        {
            return Cursor{ glfwCreateStandardCursor(GLFW_ARROW_CURSOR) };
        }
        [[nodiscard]] static Cursor createStandardCursorIBeam()
        {
            return Cursor{ glfwCreateStandardCursor(GLFW_IBEAM_CURSOR) };
        }
        [[nodiscard]] static Cursor createStandardCursorCrosshair()
        {
            return Cursor{ glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR) };
        }
        [[nodiscard]] static Cursor createStandardCursorHand()
        {
            return Cursor{ glfwCreateStandardCursor(GLFW_HAND_CURSOR) };
        }
        [[nodiscard]] static Cursor createStandardCursorHorizontalResize()
        {
            return Cursor{ glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR) };
        }
        [[nodiscard]] static Cursor createStandardCursorVerticalResize()
        {
            return Cursor{ glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR) };
        }

    };
} // namespace glfw

#endif // GLFWWRAPPER_CURSOR_HPP
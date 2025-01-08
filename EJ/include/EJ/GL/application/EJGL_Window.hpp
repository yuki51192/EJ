#ifndef EJGL_EJGLWindow_HPP
#define EJGL_EJGLWindow_HPP

//
// Tips:
//

#include "../Utils/EJGL_config.hpp"
#include "../../Utils/EJ_Utils.hpp"
#include "../../Utils/EJ_Macros.hpp"
#include "../Utils/EJGL_enum.hpp"
#include "../Camera/EJGL_Camera.hpp"

#include <glfwpp/Window.hpp>

#include <iostream>

EJGL_NAMESPACE_BEGIN

// Description:
//	This class help you to call camera callbacks
//	Key Esc is set to close the Window
//	Viewport will automatically change when framebuffer resize
class Window : public ::glfw::Window
{
private:
	using _ImplType = ::glfw::Window;

	using _KeyCode = ::glfw::KeyCode;
	using _KeyState = ::glfw::KeyState;
	using _ModifierKeyBit = ::glfw::ModifierKeyBit;

public:
    Window(int width_, int height_, const char* title_,
        const ::glfw::Monitor* monitor_ = nullptr,
        const ::glfw::Window* share_ = nullptr) :
		_ImplType{ width_, height_, title_, monitor_, share_ },
		_sizeSub{ _ImplType::sizeEvent.subscribe(CallMe::fromFunction<&onSizeEvent>()) },
		_framebufferSizeSub{ _ImplType::framebufferSizeEvent.subscribe(CallMe::fromFunction<&onFramebufferSizeEvent>()) },
		_keySub{ _ImplType::keyEvent.subscribe(CallMe::fromFunction<&onKeyEvent>()) }
    {
		_ImplType::setUserPtr(this);
	}

    Window(const Window&) = delete;
	Window(Window&& obj_) noexcept :
		_ImplType{ std::move(obj_) },
		_camera{ std::move(obj_._camera) },
		_sizeSub{ std::move(obj_._sizeSub) },
		_framebufferSizeSub{ std::move(obj_._framebufferSizeSub) },
		_keySub{ std::move(obj_._keySub) }
	{}

    Window& operator=(const Window&) = delete;
	Window& operator=(Window&& obj_) noexcept {
		_ImplType::operator=(std::move(obj_));
		_deleteCamera();
		_camera = std::move(obj_._camera);
		_sizeSub = std::move(obj_._sizeSub);
		_framebufferSizeSub = std::move(obj_._framebufferSizeSub);
		_keySub = std::move(obj_._keySub);
		return *this;
	}

	virtual ~Window() noexcept { _deleteCamera(); }

	_STD string toStringNoHeader() const noexcept {
		auto size = getSize();
		return "{ width: " + _EJ toString(size.x) +
			", height: " + _EJ toString(size.y) + " }";
	}
	_STD string toString() const noexcept {
		return _STD string("[EJ][Window]") + toStringNoHeader();
	}

	EJ_M_CONST_GET_FUNC(bool, isCameraValid, _camera != nullptr);

	template<typename CameraType = Camera>
	EJ_M_CONST_GET_FUNC(const CameraType*, getCamera, static_cast<CameraType*>(_camera));

	template<typename CameraType = Camera>
	EJ_M_GET_FUNC(CameraType*, getCamera, static_cast<CameraType*>(_camera));

	// Description:
	//	Use setCamera<Type>() to set camera to type.
	//	Type must inherit from EGL::Camera.
	// Param args_:
	//	to initilize camera, which is new Type(args_...)
	template<typename CameraType, typename... Args>
	EJ_INLINE void setCamera(Args&... args_) {
		_deleteCamera();
		_camera = new CameraType(this, args_...);
	}

	// get size before resize
	EJ_M_CONST_GET_FUNC(const glm::ivec2&, getLastSize, _lastSize);

	template <typename Type_ = float>
	auto getReverseYCursorPos() const {
		int height = getSize().y;
		auto pos = getCursorPos();
		return glm::vec2{ static_cast<Type_>(pos.x), height - static_cast<Type_>(pos.y) };
	}
	// this function is for camera
	// reverse x and move to make x = [-w/2, w/2], y = [-h/2, h/2]
	template <typename Type_ = float>
	auto getReverseXCenteredCursorPos() const {
		auto hSize = getSize() / 2;
		auto pos = getCursorPos();
		return glm::tvec2<Type_>{ hSize.x - static_cast<Type_>(pos.x),
			static_cast<Type_>(pos.y) - hSize.y };
	}

	template <typename Type_ = float>
	auto getNDCCursorPos() const {
		return getNDCCursorPos<Type_, int>(getSize());
	}
	template <typename Type_ = float, typename Type2_ = int>
	auto getNDCCursorPos(glm::tvec2<Type2_>&& size_) const {
		auto ret = this->getCursorPos();
		return glm::tvec2<Type_>{ static_cast<Type_>(ret.x) / size_.x,
			static_cast<Type_>(ret.y) / size_.y };
	}
	template <typename Type_ = float>
	auto getReverseYNDCCursorPos() const {
		auto size = getSize();
		auto ret = this->getCursorPos();
		return glm::tvec2<Type_>{ static_cast<Type_>(ret.x) / size.x,
			1 - static_cast<Type_>(ret.y) / size.y };
	}

	template <typename Type_>
	auto reversePosY(const glm::tvec2<Type_>& pos_) const {
		auto ret = pos_;
		auto height = getSize().y;
		ret.y = height - ret.y;
		return ret;
	}

private:
	Camera* _camera = nullptr;
	glm::ivec2 _lastSize = {};
	CallMe::Subscription _sizeSub;
	CallMe::Subscription _framebufferSizeSub;
	CallMe::Subscription _keySub;

	void _deleteCamera() {
		delete _camera;
		_camera = nullptr;
	}

// static
public:
	static Window& getWrapperFromHandle(GLFWwindow* handle_) {
		return getWrapperFromHandle(::glfw::Window::getWrapperFromHandle(handle_));
	}
	static Window& getWrapperFromHandle(::glfw::Window& handle_) {
		return *static_cast<Window*>(handle_.getUserPtr());
	}

	static void onSizeEvent(::glfw::Window& window_, int width_, int height_) {
		Window& w = Window::getWrapperFromHandle(window_);
		w._lastSize = glm::ivec2(width_, height_);
	}
	static void onFramebufferSizeEvent(::glfw::Window& window_, int width_, int height_) {
		glViewport(0, 0, width_, height_);
	}

	static void onKeyEvent(::glfw::Window& window_, _KeyCode keyCode_, int, _KeyState action_, _ModifierKeyBit) {
		if (keyCode_ == _KeyCode::Escape && action_ == _KeyState::Press) {
			window_.setShouldClose(true);
		}
	}

};

EJGL_NAMESPACE_END

#endif // EJGL_EJGLWindow_HPP

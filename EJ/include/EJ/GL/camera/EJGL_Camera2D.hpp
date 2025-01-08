#ifndef EJGL_CAMERA2D_HPP
#define EJGL_CAMERA2D_HPP

//
// Tips:
//	- mouse wheel to resize
//	- right click and drag to move
//	- mouse middle to return center
//	1. in defalut this camera has a ratio that 1px = 1 in world position
//	2. the ratio will change after you resize
//	3. (0, 0) will be the center of window before you move
//	4. ortho projection's center(l/2+r/2, t/2+b/2) always equal (0, 0)
//

#include "../Utils/EJGL_config.hpp"
#include "../../Utils/EJ_Macros.hpp"
#include "../Utils/EJGL_enum.hpp"

#include "EJGL_Camera.hpp"
#include "../Application/EJGL_Window.hpp"

#include <glm/vec2.hpp>

EJGL_NAMESPACE_BEGIN

class Camera2D : public Camera
{
public:
	Camera2D(Window* window_) :
		Camera{ window_ }
	{
		EJ_ASSERT(window_ != nullptr);
		auto size = window_->getSize() / 2;
		setProjectionMatrix<OrthographicMatrix>(static_cast<float>(-size.x), static_cast<float>(size.x), static_cast<float>(-size.y), static_cast<float>(size.y));
		setViewMatrix<ViewMatrix>();
		_subs.emplace_back(window_->sizeEvent.subscribe(CallMe::fromMethod<&Camera2D::_windowSizeCB>(this)));
		_subs.emplace_back(window_->mouseButtonEvent.subscribe(CallMe::fromMethod<&Camera2D::_mouseButtonCB>(this)));
		_subs.emplace_back(window_->cursorPosEvent.subscribe(CallMe::fromMethod<&Camera2D::_mouseMoveCB>(this)));
		_subs.emplace_back(window_->scrollEvent.subscribe(CallMe::fromMethod<&Camera2D::_scrollCB>(this)));
	}
	Camera2D(const Camera2D& obj_) = default;
	Camera2D(Camera2D&& obj_) noexcept = default;
	virtual ~Camera2D() = default;

	Camera2D& operator=(const Camera2D&) = default;

	_STD string toStringNoHeader() const noexcept {
		return Camera::toStringNoHeader();
	}
	_STD string toString() const noexcept {
		return "[EJ][Camera2D]" + toStringNoHeader();
	}

private:
	bool _isMoving = false;
	glm::vec2 _startMovePos = glm::vec2();
	float _invScale = 1;
	_STD vector<CallMe::Subscription> _subs;

	void _windowSizeCB(::glfw::Window& window_, int width_, int height_);
	void _mouseButtonCB(::glfw::Window& window_, MouseButton button_, MouseButtonState action_, ModifierKeyBit mods_);
	void _mouseMoveCB(::glfw::Window& window_, double x_, double y_);
	void _scrollCB(::glfw::Window& window_, double x_, double y_);

};

EJGL_NAMESPACE_END

#endif EJGL_CAMERA2D_HPP
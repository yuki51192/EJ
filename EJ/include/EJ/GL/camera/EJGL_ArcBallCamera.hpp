#ifndef EJGL_ARCBALLCAMERA_HPP
#define EJGL_ARCBALLCAMERA_HPP

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

class EJGLArcBallCamera : public Camera
{
public:
	EJGLArcBallCamera(Window* window_) :
		Camera{ window_ }
	{
		EJ_ASSERT(window_ != nullptr);
		setProjectionMatrix<PerspectiveMatrix>();
		setViewMatrix<ViewMatrix>(::glm::vec3{ 0, 0, 10 });
		_subs.emplace_back(window_->mouseButtonEvent.subscribe(CallMe::fromMethod<&EJGLArcBallCamera::_mouseButtonCB>(this)));
		_subs.emplace_back(window_->cursorPosEvent.subscribe(CallMe::fromMethod<&EJGLArcBallCamera::_mouseMoveCB>(this)));
		_subs.emplace_back(window_->scrollEvent.subscribe(CallMe::fromMethod<&EJGLArcBallCamera::_scrollCB>(this)));
	}
	EJGLArcBallCamera(const EJGLArcBallCamera& obj_) = default;
	EJGLArcBallCamera(EJGLArcBallCamera&& obj_) noexcept = default;
	virtual ~EJGLArcBallCamera() = default;

	EJGLArcBallCamera& operator=(const EJGLArcBallCamera&) = default;

	_STD string toStringNoHeader() const noexcept {
		return Camera::toStringNoHeader();
	}
	_STD string toString() const noexcept {
		return "[EJ][ArcBallCamera]" + toStringNoHeader();
	}

private:
	bool _isMoving = false;
	glm::vec2 _lastMouse = glm::vec2{};
	_STD vector<CallMe::Subscription> _subs;

private:
	glm::vec3 _screenToArcball(const glm::vec2& point) const;

	void _mouseButtonCB(::glfw::Window& window_, MouseButton button_, MouseButtonState action_, ModifierKeyBit mods_);
	void _mouseMoveCB(::glfw::Window& window_, double x_, double y_);
	void _scrollCB(::glfw::Window& window_, double x_, double y_);

};

EJGL_NAMESPACE_END

#endif EJGL_ARCBALLCAMERA_HPP
#include "EJ/GL/Camera/EJGL_ArcBallCamera.hpp"
#include "EJ/GL/Application/EJGL_Window.hpp"

#include <glm/gtx/quaternion.hpp>

EJGL_NAMESPACE_BEGIN

glm::vec3 ArcBallCamera::_screenToArcball(const glm::vec2& point) const {
	glm::ivec2 wSize = getWindow()->getSize();
	glm::vec2 p = point / glm::vec2(wSize.x, wSize.y) * 2.0f - glm::vec2(1.0f);
	float dist = glm::dot(p, p);
	if (dist <= 1.0f) {
		// point in ball
		return glm::vec3(p, sqrt(1.0f - dist));
	}
	else {
		// point out ball
		return glm::normalize(glm::vec3(p, 0.0f));
	}
}

void ArcBallCamera::_mouseButtonCB(::glfw::Window& window_, MouseButton button_, MouseButtonState action_, ModifierKeyBit mods_) {
	if (button_ == glfw::MouseButton::Right) {
		if (action_ == glfw::MouseButtonState::Press) {
			_isMoving = true;
			_lastMouse = Window::getWrapperFromHandle(window_).getReverseYCursorPos();
		}
		else if (action_ == glfw::MouseButtonState::Release) {
			_isMoving = false;
		}
	}
}

void ArcBallCamera::_mouseMoveCB(::glfw::Window& window_, double x_, double y_) {
	if (!_isMoving)
		return;

	glm::vec2 pos = Window::getWrapperFromHandle(window_).reversePosY<float>({ x_, y_ });
	if (window_.getKey(::glfw::KeyCode::LeftAlt) || window_.getKey(::glfw::KeyCode::RightAlt)) {
		// pan
		glm::vec3 eye = getViewMatrix()->getEye();
		getViewMatrix()->setEye(eye + eye.z * 0.001f * ::glm::vec3(_lastMouse - pos, 0.0f));
	}
	else {
		// rotate
		glm::vec3 ballPos = _screenToArcball(pos);

		glm::quat rotation = getViewMatrix()->getRotation();
		glm::quat deltaQuat = glm::rotation(ballPos, _screenToArcball(_lastMouse));
		getViewMatrix()->setRotation(rotation * deltaQuat);
	}
	_lastMouse = pos;
	getViewMatrix()->update();
}

void ArcBallCamera::_scrollCB(::glfw::Window& window_, double x_, double y_) {
	glm::vec3& eye = getViewMatrix()->eye();
	float scale = 1.f - 0.1f * static_cast<float>(y_);
	eye.z *= scale;
	if (eye.z < 0.01f) {
		eye.z = 0.01f;
	}
	getViewMatrix()->update();
}

EJGL_NAMESPACE_END
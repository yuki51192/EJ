#include "EJ/GL/Camera/EJGL_Camera2D.hpp"
#include "EJ/GL/Application/EJGL_Window.hpp"

#include "EJ/GL/Helper/EJGL_GLMExtension.hpp"

EJGL_NAMESPACE_BEGIN

void Camera2D::_windowSizeCB(::glfw::Window& window_, int width_, int height_) {
	glm::ivec2 tmp = getWindow()->getLastSize();
	auto ortho = getProjectionMatrix<OrthographicMatrix>();
	float hW = (width_ >> 1) * _invScale, hH = (height_ >> 1) * _invScale;
	ortho->setLeft(-hW);
	ortho->setRight(hW);
	ortho->setTop(hH);
	ortho->setBottom(-hH);
	ortho->update();
}
void Camera2D::_mouseButtonCB(::glfw::Window& window_, Camera2D::MouseButton button_, Camera2D::MouseButtonState action_, Camera2D::ModifierKeyBit mods_) {
	if (button_ == ::glfw::MouseButton::Right) {
		if (action_ == ::glfw::MouseButtonState::Press) {
			_isMoving = true;
			_startMovePos = Window::getWrapperFromHandle(window_).getReverseYCursorPos();
		}
		else if (action_ == ::glfw::MouseButtonState::Release) {
			_isMoving = false;
		}
	}
}
void Camera2D::_mouseMoveCB(::glfw::Window& window_, double x_, double y_) {
	if (!_isMoving)
		return;
	Window& win = Window::getWrapperFromHandle(window_);
	::glm::vec2 pos = win.reversePosY<float>({ x_, y_ });
	::glm::vec3 eye = getViewMatrix()->getEye();
	eye += ::glm::vec3{ ::glm::vec2{ _startMovePos - pos } *_invScale, 0 };
	getViewMatrix()->setEye(eye);
	_startMovePos = ::glm::vec2{ pos };

	// move eye to make it like we resize at mouse pos
	auto mouse = win.getReverseXCenteredCursorPos() * _invScale;
	auto eyeScreenPos = ::glm::vec2(getViewMatrix()->getEye());
	// TODO
}

void Camera2D::_scrollCB(::glfw::Window& window_, double x_, double y_) {
	auto ortho = getProjectionMatrix<OrthographicMatrix>();
	auto view = getViewMatrix();

	// resize
	constexpr float _OffsetScale = 0.1f;
	float y = _OffsetScale * static_cast<float>(y_);
	float scale = 1 - y;
	ortho->setLeft(ortho->getLeft() * scale);
	ortho->setRight(ortho->getRight() * scale);
	ortho->setTop(ortho->getTop() * scale);
	ortho->setBottom(ortho->getBottom() * scale);
	ortho->update();

	// move eye to make it like we resize at mouse pos
	auto mouse = Window::getWrapperFromHandle(window_).getReverseXCenteredCursorPos() * _invScale;
	auto eyeScreenPos = ::glm::vec2(view->getEye());

	// y == 1 - scale
	view->setEye(::glm::vec3((eyeScreenPos - mouse) * y + eyeScreenPos, 1));

	_invScale *= scale;
}

EJGL_NAMESPACE_END
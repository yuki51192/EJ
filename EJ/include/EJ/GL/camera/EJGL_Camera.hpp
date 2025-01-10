#ifndef EJGL_CAMERA_HPP
#define EJGL_CAMERA_HPP

//
// Tips:
//

#include "../Utils/EJGL_config.hpp"
#include "../../Utils/EJ_Macros.hpp"
#include "../Utils/EJGL_enum.hpp"

#include "../Matrix/EJGL_ProjectionMatrix.hpp"
#include "../Matrix/EJGL_ViewMatrix.hpp"

#include <glfwpp/Window.hpp>
#include <functional>
#include <bitset>

EJGL_NAMESPACE_BEGIN

class Window;

class Camera
{
public:
	using WindowSizeCBT = _STD function<void(Window&, int, int)>;

	using KeyCode = ::glfw::KeyCode;
	using KeyState = ::glfw::KeyState;
	using ModifierKeyBit = ::glfw::ModifierKeyBit;
	using MouseButton = ::glfw::MouseButton;
	using MouseButtonState = ::glfw::MouseButtonState;

public:
	Camera(Window* window_) :
		_window{ window_ },
		_view{ nullptr }, 
		_projection{ nullptr }
	{}
	Camera(const Camera& obj_) :
		_window{ obj_._window },
		_view{ obj_._view != nullptr ?
			obj_._view->clone() : nullptr},
		_projection{ obj_._projection != nullptr ?
			obj_._projection->clone() : nullptr}
	{}
	Camera(Camera&& obj_) noexcept :
		_window{ std::move(obj_._window) },
		_view{ std::move(obj_._view) },
		_projection{ std::move(obj_._projection) }
	{
		obj_._view = nullptr;
		obj_._projection = nullptr;
	}
	virtual ~Camera() {
		_deleteProjectionMatrix();
		_deleteViewMatrix();
	}

	Camera& operator=(const Camera& obj_) {
		_window = obj_._window;
		_deleteViewMatrix();
		_view = obj_._view != nullptr ?
			obj_._view->clone() : nullptr;
		_deleteProjectionMatrix();
		_projection = obj_._projection != nullptr ?
			obj_._projection->clone() : nullptr;
		return *this;
	}

	_STD string toStringNoHeader() const noexcept {
		return "{ view: " + _view->toString() + ",\nprojection : " + _projection->toString() + " }";
	}
	_STD string toString() const noexcept {
		return "[EJ][Camera]" + toStringNoHeader();
	}

	void swap(Camera& obj_) noexcept {
		_STD swap(_window, obj_._window);
		_STD swap(_view, obj_._view);
		_STD swap(_projection, obj_._projection);
	}

	EJ_M_CONST_GET_FUNC(bool, isWindowValid, _window != nullptr);
	EJ_M_CONST_GET_FUNC(bool, isViewMatrixValid, _view != nullptr);
	EJ_M_CONST_GET_FUNC(bool, isProjectionMatrixValid, _projection != nullptr);

	EJ_M_CONST_GET_FUNC(const Window*, getWindow, _window);
	template<typename ViewMatrixType = ViewMatrix>
	EJ_M_CONST_GET_FUNC(const ViewMatrixType*, getViewMatrix, static_cast<ViewMatrixType*>(_view));
	template<typename ProjectionMatrixType = ProjectionMatrix>
	EJ_M_CONST_GET_FUNC(const ProjectionMatrixType*, getProjectionMatrix, static_cast<ProjectionMatrixType*>(_projection));

	EJ_M_GET_FUNC(Window*, getWindow, _window);
	template<typename ViewMatrixType = ViewMatrix>
	EJ_M_GET_FUNC(ViewMatrixType*, getViewMatrix, static_cast<ViewMatrixType*>(_view));
	template<typename ProjectionMatrixType = ProjectionMatrix>
	EJ_M_GET_FUNC(ProjectionMatrixType*, getProjectionMatrix, static_cast<ProjectionMatrixType*>(_projection));

	template<typename ViewMatrixType, typename... Args>
	// Description:
	//	Use setViewMatrix<Type>() to set viewMatrix to type.
	//	Type must inherit from EGL::ViewMatrix.
	// Param args_:
	//	to initilize view, which is new Type(args_...)
	void setViewMatrix(Args... args_) {
		_deleteViewMatrix();
		_view = new ViewMatrixType(args_...);
	}
	template<typename ProjectionMatrixType, typename... Args>
	// Description:
	//	Use setProjectionMatrix<Type>() to set projectionMatrix to type.
	//	Type must inherit from EGL::ProjectionMatrix.
	// Param args_:
	//	to initilize projection, which is new Type(args_...)
	void setProjectionMatrix(Args... args_) {
		_deleteProjectionMatrix();
		_projection = new ProjectionMatrixType(args_...);
	}

	// about camera
	//virtual EJ_M_CONST_GET_FUNC(glm::vec3, getCameraPos, _scrollCB);

	// you need to call update() before this, or it might be wrong
	glm::vec3 screen2World(const glm::vec2& scPos_) const {
		return _view->toGlm() * _projection->toGlm() * glm::vec4(scPos_, 0, 1);
	}

private:
	Window* _window;
	ViewMatrix* _view;
	ProjectionMatrix* _projection;

	void _deleteViewMatrix() {
		delete _view;
	}
	void _deleteProjectionMatrix() {
		delete _projection;
	}

};

EJGL_NAMESPACE_END

#endif // EJGL_CAMERA_HPP
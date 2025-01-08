#include "EJ/GL/Matrix/EJGL_ProjectionMatrix.hpp"
#include "EJ/GL/Helper/EJGL_GLMExtension.hpp"

EJGL_NAMESPACE_BEGIN

// ProjectionMatrix
ProjectionMatrix::ProjectionMatrix(const ::glm::mat4& mat_) :
	::glm::mat4{ mat_ },
	_hasChange{ true }
{}

_STD string ProjectionMatrix::toStringNoHeader() const noexcept {
	return "{ m: " + _EJ toString(static_cast<::glm::mat4>(*this)) + " }";
}
_STD string ProjectionMatrix::toString() const noexcept {
	return _STD string("[EJ][ProjectionMatrix]") + toStringNoHeader();
}

// PerspectiveMatrix
PerspectiveMatrix::PerspectiveMatrix(float fov_, float aspectRatio_, float near_, float far_) :
	ProjectionMatrix{ s_getMatrix(fov_, aspectRatio_, near_, far_) },
	_fov{ fov_ }, _aspectRatio{ aspectRatio_ }, _near{ near_ }, _far{ far_ }
{}

_STD string PerspectiveMatrix::toStringNoHeader() const noexcept {
	return "{ fov: " + _EJ toString(_fov) + ", aspect ratio : " + _EJ toString(_aspectRatio) +
		", near: " + _EJ toString(_near) + ", far: " + _EJ toString(_far) + ", " + ProjectionMatrix::toStringNoHeader() + " }";
}
_STD string PerspectiveMatrix::toString() const noexcept {
	return _STD string("[EJ][PerspectiveMatrix]") + toStringNoHeader();
}

// EJGLOrthographicMatrix
OrthographicMatrix::OrthographicMatrix(float left_, float right_, float bottom_, float top_, float near_, float far_) :
	ProjectionMatrix{ s_getMatrix(left_, right_, bottom_, top_, near_, far_) },
	_left{ left_ }, _right{ right_ },
	_bottom{ bottom_ }, _top{ top_ },
	_near{ near_ }, _far{ far_ }
{}

_STD string OrthographicMatrix::toStringNoHeader() const noexcept {
	return "left: " + _EJ toString(_left) + ", right : " + _EJ toString(_right) +
		", bottom: " + _EJ toString(_bottom) + ", top: " + _EJ toString(_top) +
		", near: " + _EJ toString(_near) + ", far: " + _EJ toString(_far) + ", " + ProjectionMatrix::toStringNoHeader() + " }";
}
_STD string OrthographicMatrix::toString() const noexcept {
	return _STD string("[EJ][OrthographicMatrix]") + toStringNoHeader();
}

EJGL_NAMESPACE_END
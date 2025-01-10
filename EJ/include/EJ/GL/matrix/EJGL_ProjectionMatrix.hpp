#ifndef EJGL_ProjectionMatrix_HPP
#define EJGL_ProjectionMatrix_HPP

//
// Tips:
//

#include "../Utils/EJGL_config.hpp"
#include "../../Utils/EJ_Macros.hpp"
#include "../Utils/EJGL_enum.hpp"

#include <bitset>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

EJGL_NAMESPACE_BEGIN

class ProjectionMatrix : public ::glm::mat4
{
public:
	ProjectionMatrix(const glm::mat4& mat_ = glm::mat4(1));
	ProjectionMatrix(const ProjectionMatrix&) = default;
	ProjectionMatrix(ProjectionMatrix&&) noexcept = default;
	virtual ~ProjectionMatrix() = default;

	ProjectionMatrix& operator=(const ProjectionMatrix&) = default;

	_STD string toStringNoHeader() const noexcept;
	_STD string toString() const noexcept;

	EJ_M_CONST_GET_FUNC(const ::glm::mat4&, toGlm, *this);

	virtual ProjectionMatrix* clone() const {
		return new ProjectionMatrix(*this);
	}

	bool hasChange() const noexcept {
		if (_hasChange) {
			_hasChange = false;
			return true;
		} return false;
	}
	virtual void update() { _hasChange = true; }

private:
	mutable bool _hasChange = false;

};

class PerspectiveMatrix : public ProjectionMatrix {
public:
	PerspectiveMatrix(float fov_ = 45.f, float aspectRatio_ = 1.f, float near_ = .1f, float far_ = 100.f);
	PerspectiveMatrix(const PerspectiveMatrix&) = default;
	PerspectiveMatrix(PerspectiveMatrix&&) noexcept = default;
	virtual ~PerspectiveMatrix() = default;

	PerspectiveMatrix& operator=(const PerspectiveMatrix&) = default;

	_STD string toStringNoHeader() const noexcept;
	_STD string toString() const noexcept;

	virtual PerspectiveMatrix* clone() const {
		return new PerspectiveMatrix(*this);
	}

	EJ_M_GET_FUNC(float&, fov, _fov);
	EJ_M_GET_FUNC(float&, aspectRatio, _aspectRatio);
	EJ_M_GET_FUNC(float&, near, _near);
	EJ_M_GET_FUNC(float&, far, _far);

	EJ_M_CONST_GET_FUNC(float, getFov, _fov);
	EJ_M_CONST_GET_FUNC(float, getAspectRatio, _aspectRatio);
	EJ_M_CONST_GET_FUNC(float, getNear, _near);
	EJ_M_CONST_GET_FUNC(float, getFar, _far);

	EJ_M_SET_FUNC(float, setFov, _fov = val_);
	EJ_M_SET_FUNC(float, setAspectRatio, _aspectRatio = val_);
	EJ_M_SET_FUNC(float, setNear, _near = val_);
	EJ_M_SET_FUNC(float, setFar, _far = val_);

	virtual void update() override {
		::glm::mat4::operator=(_getMatrix());
		ProjectionMatrix::update();
	}

private:
	float _fov;
	float _aspectRatio;
	float _near;
	float _far;

	EJ_M_CONST_GET_FUNC(glm::mat4, _getMatrix, s_getMatrix(_fov, _aspectRatio, _near, _far));

private:
	EJ_INLINE static ::glm::mat4 s_getMatrix(float fov_, float aspect_, float near_, float far_) {
		return ::glm::perspective(fov_, aspect_, near_, far_);
	}

};

class OrthographicMatrix : public ProjectionMatrix {
public:
	OrthographicMatrix(float left_ = -1, float right_ = 1, float bottom_ = -1, float top_ = 1, float near_ = 0.1, float far_ = 10);
	OrthographicMatrix(const OrthographicMatrix&) = default;
	OrthographicMatrix(OrthographicMatrix&&) noexcept = default;
	virtual ~OrthographicMatrix() = default;

	OrthographicMatrix& operator=(const OrthographicMatrix&) = default;

	_STD string toStringNoHeader() const noexcept;
	_STD string toString() const noexcept;

	virtual OrthographicMatrix* clone() const {
		return new OrthographicMatrix(*this);
	}

	EJ_M_GET_FUNC(float&, left, _left);
	EJ_M_GET_FUNC(float&, right, _right);
	EJ_M_GET_FUNC(float&, bottom, _bottom);
	EJ_M_GET_FUNC(float&, top, _top);
	EJ_M_GET_FUNC(float&, near, _near);
	EJ_M_GET_FUNC(float&, far, _far);

	EJ_M_CONST_GET_FUNC(float, getLeft, _left);
	EJ_M_CONST_GET_FUNC(float, getRight, _right);
	EJ_M_CONST_GET_FUNC(float, getBottom, _bottom);
	EJ_M_CONST_GET_FUNC(float, getTop, _top);
	EJ_M_CONST_GET_FUNC(float, getNear, _near);
	EJ_M_CONST_GET_FUNC(float, getFar, _far);

	EJ_M_SET_FUNC(float, setLeft, _left = val_);
	EJ_M_SET_FUNC(float, setRight, _right = val_);
	EJ_M_SET_FUNC(float, setBottom, _bottom = val_);
	EJ_M_SET_FUNC(float, setTop, _top = val_);
	EJ_M_SET_FUNC(float, setNear, _near = val_);
	EJ_M_SET_FUNC(float, setFar, _far = val_);

	EJ_INLINE virtual void update() override {
		::glm::mat4::operator=(_getMatrix());
		ProjectionMatrix::update();
	}

private:
	float _left;
	float _right;
	float _bottom;
	float _top;
	float _near;
	float _far;

	EJ_M_CONST_GET_FUNC(glm::mat4, _getMatrix, s_getMatrix(_left, _right, _bottom, _top, _near, _far));

private:
	EJ_INLINE static glm::mat4 s_getMatrix(float left_, float right_, float bottom_, float top_, float near_, float far_) {
		return ::glm::ortho(left_, right_, bottom_, top_, near_, far_);
	}

};

EJGL_NAMESPACE_END

#endif // EJGL_ProjectionMatrix_HPP
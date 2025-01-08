#ifndef EJGL_ViewMatrix_HPP
#define EJGL_ViewMatrix_HPP

//
// Tips:
//

#include "../Utils/EJGL_config.hpp"
#include "../../Utils/EJ_Macros.hpp"
#include "../Utils/EJGL_enum.hpp"
#include "../Helper/EJGL_GLMExtension.hpp"

#include <bitset>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

EJGL_NAMESPACE_BEGIN

class ViewMatrix : public ::glm::mat4
{
public:
	ViewMatrix(const glm::vec3& eye_ = { 0, 0, 1 }, const glm::quat& rotation_ = { 1, 0, 0, 0 }) :
		_hasChange{ true },
		_eye{ eye_ },
		_rotation{ rotation_ }
	{}
	ViewMatrix(const ViewMatrix&) = default;
	ViewMatrix(ViewMatrix&&) noexcept = default;
	~ViewMatrix() = default;

	ViewMatrix& operator=(const ViewMatrix&) = default;

	_STD string toStringNoHeader() const noexcept {
		return "{ eye: " + _EJ toString(_eye) + ", rotation : " +
			_EJ toString(_rotation) + ", { m: " + _EJ toString(static_cast<::glm::mat4>(*this)) + " } }";
	}
	_STD string toString() const noexcept {
		return _STD string("[EJ][ViewMatrix]") + toStringNoHeader();
	}

	EJ_M_GET_FUNC(::glm::mat4&, toGlm, *this);
	EJ_M_CONST_GET_FUNC(const ::glm::mat4&, toGlm, *this);

	virtual ViewMatrix* clone() const {
		return new ViewMatrix(*this);
	}

	// about view
	EJ_M_GET_FUNC(glm::vec3&, eye, _eye);
	EJ_M_GET_FUNC(glm::quat&, rotation, _rotation);

	EJ_M_CONST_GET_FUNC(const glm::vec3&, getEye, _eye);
	EJ_M_CONST_GET_FUNC(const glm::quat&, getRotation, _rotation);
	EJ_M_CONST_GET_FUNC(const glm::vec3, getRotationEuler, ::glm::eulerAngles(_rotation));

	EJ_M_SET_FUNC(const glm::vec3&, setEye, _eye = val_);
	EJ_M_SET_FUNC(const glm::quat&, setRotation, _rotation = val_);
	EJ_M_SET_FUNC(const glm::vec3&, setRotationEuler, _rotation = glm::quat{ val_ });

	bool hasChange() const noexcept {
		if (_hasChange) {
			_hasChange = false;
			return true;
		} return false;
	}
	EJ_INLINE virtual void update() {
		glm::mat4::operator=(glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-_eye.x, -_eye.y, -_eye.z, 1.0f));
		glm::mat4::operator*=(glm::mat4_cast(glm::inverse(_rotation)));
		_hasChange = true;
	}

private:
	mutable bool _hasChange = false;
	glm::vec3 _eye;
	glm::quat _rotation;

};

EJGL_NAMESPACE_END

#endif // EJGL_ViewMatrix_HPP
#ifndef EJ_VECTOR_HPP
#define EJ_VECTOR_HPP

#include "../utils/EJ_config.hpp"
#include "../utils/EJ_Math.hpp"
#include "../utils/EJ_Utils.hpp"
#include "../Containers/EJ_StaticArray.hpp"

#include <functional>

EJ_NAMESPACE_BEGIN

#define __M_OP_FUNC_TEMPLATE(op) \
	template<typename OtherScalarType_, \
	typename = _STD enable_if_t<_STD is_convertible_v< \
		decltype(_STD declval<ScalarType>() op _STD declval<OtherScalarType_>()), ScalarType>>>
#define __M_OP_SCALAR_EQ_FUNC(op, opFunc) \
	__M_OP_FUNC_TEMPLATE(op) \
	constexpr const _ThisType& operator op##= (const OtherScalarType_& s_) { \
		_STD transform(_ImplType::cbegin(), _ImplType::cend(), _ImplType::begin(), \
			_STD bind(opFunc, _STD placeholders::_1, s_)); \
		return *this; \
	}
#define __M_OP_SCALAR_FUNC(op) \
	__M_OP_FUNC_TEMPLATE(op) \
	constexpr _ThisType operator op (const OtherScalarType_& s_) const { \
		return const_cast<_ThisType&>(_ThisType(*this) op##= s_); \
	}
#define __M_OP_EQ_FUNC(op, opFunc) \
	__M_OP_FUNC_TEMPLATE(op) \
	constexpr const _ThisType& operator op##= (const Vector<OtherScalarType_, _Dim>& obj_) { \
		_STD transform(_ImplType::cbegin(), _ImplType::cend(), obj_.cbegin(), _ImplType::begin(), opFunc); \
		return *this; \
	}
#define __M_OP_FUNC(op) \
	__M_OP_FUNC_TEMPLATE(op) \
	constexpr _ThisType operator op (const Vector<OtherScalarType_, _Dim>& obj_) const { \
		return const_cast<_ThisType&>(_ThisType(*this) op##= obj_); \
	}


template <typename _ScalarType, _STD size_t _Dim>
class Vector : private StaticArray<_ScalarType, _Dim> {
	static_assert(_Dim >= 1, "[EJ][Vector][Error]: Vector Dim Can't Be Zero");
public:
	using ScalarType = _ScalarType;
	using SizeType = _STD size_t;

private:
	using _ImplType = StaticArray<ScalarType, _Dim>;
	using _ThisType = Vector<ScalarType, _Dim>;

public:
	constexpr Vector() : _ImplType() {}
	template<typename OtherScalarType_, typename... OtherScalarTypes_,
		typename = typename _STD enable_if_t<_EJ are_convertible_v<ScalarType, OtherScalarType_, OtherScalarTypes_...>>>
	constexpr Vector(OtherScalarType_ v_, OtherScalarTypes_... vs_) : _ImplType({ static_cast<ScalarType>(v_), static_cast<ScalarType>(vs_)... }) {}

	template <typename OtherScalarType_, SizeType OtherDim_,
		typename = _STD enable_if_t<_STD is_convertible_v<OtherScalarType_, ScalarType>>>
	constexpr explicit Vector(const Vector<OtherScalarType_, OtherDim_>& obj_) {
		this->operator=(obj_);
	}
	constexpr Vector(const _ThisType& obj_) = default;
	constexpr Vector(_ThisType&& obj_) noexcept = default;

	template <typename OtherScalarType_, SizeType OtherDim_,
		typename = _STD enable_if<_STD is_convertible_v<OtherScalarType_, ScalarType>>>
	constexpr _ThisType& operator=(const Vector<OtherScalarType_, OtherDim_>& obj_) {
		if constexpr (OtherDim_ <= _Dim) {
			_STD transform(obj_.cbegin(), obj_.cend(), _ImplType::begin(),
				[](OtherScalarType_ s_) {
					return static_cast<ScalarType>(s_);
				});
			_STD fill(_ImplType::begin() + OtherDim_, _ImplType::end(), 0);
		}
		else
			_STD transform(obj_.cbegin(), obj_.cbegin() + _Dim, _ImplType::begin(),
				[](OtherScalarType_ s_) {
					return static_cast<ScalarType>(s_);
				});
		return *this;
	}
	_ThisType& operator=(const _ThisType& obj_) noexcept = default;
	_ThisType& operator=(_ThisType&& obj_) noexcept = default;

	~Vector() = default;

	using _ImplType::swap;

	using _ImplType::data;
	static consteval SizeType size() { return _Dim; }

	_STD string toStringNoHeader() const noexcept {
		return _EJ toStringNoHeader(static_cast<_ImplType>(*this));
	}
	_STD string toString() const noexcept {
		return _STD string("[EJ][Vector]") + toStringNoHeader();
	}

	template <typename ToScalarType_, SizeType ToSize_ = _Dim>
	Vector<ToScalarType_, ToSize_> to() {
		return Vector<ToScalarType_, ToSize_>(*this);
	}

	using _ImplType::operator[];
	constexpr auto operator==(const _ThisType& obj_) const {
		return static_cast<_ImplType>(*this) == static_cast<_ImplType>(obj_);
	}
	constexpr auto operator<=>(const _ThisType& obj_) const {
		return static_cast<_ImplType>(*this) <=> static_cast<_ImplType>(obj_);
	}
	__M_OP_SCALAR_EQ_FUNC(*, _STD multiplies{});
	__M_OP_SCALAR_EQ_FUNC(/, _STD divides{});
	__M_OP_SCALAR_FUNC(*);
	__M_OP_SCALAR_FUNC(/);
	__M_OP_EQ_FUNC(+, _STD plus{});
	__M_OP_EQ_FUNC(-, _STD minus{});
	__M_OP_EQ_FUNC(*, _STD multiplies{});
	__M_OP_EQ_FUNC(/, _STD divides{});
	__M_OP_FUNC(+);
	__M_OP_FUNC(-);
	__M_OP_FUNC(*);
	__M_OP_FUNC(/);
	constexpr _ThisType operator-() const {
		_ThisType temp{};
		_STD transform(_ImplType::cbegin(), _ImplType::cend(), temp.begin(), _STD negate<>());
		return temp;
	}

	constexpr decltype(_STD declval<ScalarType>() + _STD declval<ScalarType>()) l1Norm() const {
		return _STD reduce(_ImplType::cbegin() + 1, _ImplType::cend(), _ImplType::front(), _STD plus{});
	}

	constexpr auto sqrNorm() const {
		return _STD transform_reduce(_ImplType::cbegin() + 1, _ImplType::cend(), _EJ Square{}(_ImplType::front()),
			_STD plus{}, _EJ Square{});
	}

	constexpr auto length() const {
		return _STD sqrt(sqrNorm());
	}

	template <typename OtherScalarType_>
	constexpr auto dot(const Vector<OtherScalarType_, _Dim>& obj_) const {
		return _STD inner_product(_ImplType::cbegin(), _ImplType::cend(), obj_.cbegin(), 0);
	}

	template <typename OtherScalarType_, typename = _STD enable_if<_Dim == 3>>
	constexpr auto cross(const Vector<OtherScalarType_, _Dim>& obj_) const {
		return Vector<decltype(_STD declval<ScalarType>() * _STD declval<OtherScalarType_>()), 3> {
			(*this)[1] * obj_[2] - (*this)[2] * obj_[1],
			(*this)[2] * obj_[0] - (*this)[0] * obj_[2],
			(*this)[0] * obj_[1] - (*this)[1] * obj_[0]
		};
	}

	const _ThisType& normalize() {
		return operator/=(length());
	}

	const _ThisType normalized() const & {
		return operator/(length());
	}

	constexpr ScalarType max() const {
		return *_STD max_element(cbegin(), cend());
	}
	ScalarType absMax() const {
		return _STD abs(
			*_STD max_element(cbegin(), cend(),
				[](const ScalarType& v1_, const ScalarType& v2_) {
					return _STD abs(v1_) < _STD abs(v2_);
				}));
	}
	constexpr ScalarType min() const {
		return *_STD min_element(cbegin(), cend());
	}
	ScalarType absMin() const {
		return _STD abs(
			*_STD min_element(cbegin(), cend(),
				[](const ScalarType& v1_, const ScalarType& v2_) {
					return _STD abs(v1_) < _STD abs(v2_);
				}));
	}

	constexpr auto mean() -> decltype(l1Norm() / _Dim) const {
		return l1Norm() / _Dim;
	}

	using _ImplType::begin;
	using _ImplType::rbegin;
	using _ImplType::cbegin;
	using _ImplType::crbegin;
	using _ImplType::end;
	using _ImplType::rend;
	using _ImplType::cend;
	using _ImplType::crend;

private:

};

#undef __M_OP_FUNC_TEMPLATE
#undef __M_OP_SCALAR_EQ_FUNC
#undef __M_OP_SCALAR_FUNC
#undef __M_OP_EQ_FUNC
#undef __M_OP_FUNC

using Vec2i = Vector<int, 2>;
using Vec2u = Vector<unsigned int, 2>;
using Vec2f = Vector<float, 2>;
using Vec2d = Vector<double, 2>;
using Vec3i = Vector<int, 3>;
using Vec3u = Vector<unsigned int, 3>;
using Vec3f = Vector<float, 3>;
using Vec3d = Vector<double, 3>;
using Vec4i = Vector<int, 4>;
using Vec4u = Vector<unsigned int, 4>;
using Vec4f = Vector<float, 4>;
using Vec4d = Vector<double, 4>;

EJ_NAMESPACE_END

/************************************************************************/
/*                             TEST SOURCE                              */
/************************************************************************/
/*

using namespace EJ;
int main() {
#define TEST_AND_LOG(TEST_) \
	std::cout << std::setw(35) << #TEST_ ": " << (TEST_) << '\n'

#define LOG_LINE \
	std::cout << "--------------------------------------------------------------------------------------\n"

	// test constructors
	LOG_LINE;
	std::cout << "Constructors\n";
	LOG_LINE;
	TEST_AND_LOG(Vec3f{});
	TEST_AND_LOG(Vec3f(1));
	TEST_AND_LOG(Vec3f(1, 2, 3));
	TEST_AND_LOG(Vec3f(0.1, 0.2, 0.3));
	TEST_AND_LOG(Vec3f(Vec3f(1, 2, 3)));
	TEST_AND_LOG(Vec3f(std::move(Vec3f(1, 2, 3))));

	Vec3f v1{ 1, 2, 3 };
	TEST_AND_LOG(v1);
	Vec3i v2{ 4, 5, 6 };
	TEST_AND_LOG(v2);
	Vec3u v3{ 7, 8, 9 };
	TEST_AND_LOG(v3);

	// test assignment operators
	LOG_LINE;
	std::cout << "Assignment operators\n";
	LOG_LINE;
	TEST_AND_LOG(v3 = v1);
	TEST_AND_LOG(v3 = std::move(Vec3i(v2)));
	TEST_AND_LOG(v3 = Vec2d(1, 2));
	TEST_AND_LOG(v3 = Vec3d(1, 2, 3));
	TEST_AND_LOG(v3 = Vec4d(1, 2, 3, 4));

	// test scalar operators
	LOG_LINE;
	std::cout << "Scalar operators\n";
	LOG_LINE;
	TEST_AND_LOG(-v1);
	TEST_AND_LOG(v1 * 10.0);
	TEST_AND_LOG(v1 *= 10.0);
	TEST_AND_LOG(v1 / 10.0);
	TEST_AND_LOG(v1 /= 10.0);

	// test Compute operators
	LOG_LINE;
	std::cout << "Compute operators\n";
	LOG_LINE;
	TEST_AND_LOG(v1 + v2);
	TEST_AND_LOG(v1 - v2);
	TEST_AND_LOG(v1 * v2);
	TEST_AND_LOG(v1 / v2);
	TEST_AND_LOG(v1 += v2);
	TEST_AND_LOG(v1 -= v2);
	TEST_AND_LOG(v1 *= v2);
	TEST_AND_LOG(v1 /= v2);
	TEST_AND_LOG(v1.dot(v2));
	TEST_AND_LOG(v1.cross(v2));

	// test compare operators
	LOG_LINE;
	std::cout << "Compare operators\n";
	LOG_LINE;
	TEST_AND_LOG(v1 < v2.to<float>());
	TEST_AND_LOG(v1 == v2.to<float>());
	TEST_AND_LOG(v1 > v2.to<float>());

	// test other functions
	LOG_LINE;
	std::cout << "Other functions\n";
	LOG_LINE;
	TEST_AND_LOG(v1.l1Norm());
	TEST_AND_LOG(v1.sqrNorm());
	TEST_AND_LOG(v1.length());
	TEST_AND_LOG(v1.normalized());
	TEST_AND_LOG(v1);
	TEST_AND_LOG(v1.normalize());
	TEST_AND_LOG(v1);
	TEST_AND_LOG(v1.max());
	TEST_AND_LOG(v1.min());
	TEST_AND_LOG(v1.mean());

	// test raw pointer
	LOG_LINE;
	std::cout << "Raw pointer\n";
	LOG_LINE;
	auto p = v1.data();
	TEST_AND_LOG(p[0]);
	TEST_AND_LOG(p[1]);
	TEST_AND_LOG(p[2]);
}

 */

#endif // EJ_VECTOR_HPP
 
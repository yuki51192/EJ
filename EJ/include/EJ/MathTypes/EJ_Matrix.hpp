#ifndef EJ_MATRIX_HPP
#define EJ_MATRIX_HPP

#include "../utils/EJ_config.hpp"
#include "../utils/EJ_Math.hpp"
#include "../utils/EJ_Utils.hpp"
#include "../utils/EJ_Macros.hpp"
#include "../MathTypes/EJ_Vector.hpp"
#include "../MathTypes/EJ_MatrixTraits.hpp"

namespace {
	template <typename MatrixType_>
	constexpr const MatrixType_& _inverseImpl44(MatrixType_& m_) {
		auto Coef00 = m_[2][2] * m_[3][3] - m_[3][2] * m_[2][3];
		auto Coef02 = m_[1][2] * m_[3][3] - m_[3][2] * m_[1][3];
		auto Coef03 = m_[1][2] * m_[2][3] - m_[2][2] * m_[1][3];

		auto Coef04 = m_[2][1] * m_[3][3] - m_[3][1] * m_[2][3];
		auto Coef06 = m_[1][1] * m_[3][3] - m_[3][1] * m_[1][3];
		auto Coef07 = m_[1][1] * m_[2][3] - m_[2][1] * m_[1][3];

		auto Coef08 = m_[2][1] * m_[3][2] - m_[3][1] * m_[2][2];
		auto Coef10 = m_[1][1] * m_[3][2] - m_[3][1] * m_[1][2];
		auto Coef11 = m_[1][1] * m_[2][2] - m_[2][1] * m_[1][2];

		auto Coef12 = m_[2][0] * m_[3][3] - m_[3][0] * m_[2][3];
		auto Coef14 = m_[1][0] * m_[3][3] - m_[3][0] * m_[1][3];
		auto Coef15 = m_[1][0] * m_[2][3] - m_[2][0] * m_[1][3];

		auto Coef16 = m_[2][0] * m_[3][2] - m_[3][0] * m_[2][2];
		auto Coef18 = m_[1][0] * m_[3][2] - m_[3][0] * m_[1][2];
		auto Coef19 = m_[1][0] * m_[2][2] - m_[2][0] * m_[1][2];

		auto Coef20 = m_[2][0] * m_[3][1] - m_[3][0] * m_[2][1];
		auto Coef22 = m_[1][0] * m_[3][1] - m_[3][0] * m_[1][1];
		auto Coef23 = m_[1][0] * m_[2][1] - m_[2][0] * m_[1][1];

		using ScalarType = typename MatrixType_::ScalarType;

		_EJ Vector<ScalarType, 4> Fac0(Coef00, Coef00, Coef02, Coef03);
		_EJ Vector<ScalarType, 4> Fac1(Coef04, Coef04, Coef06, Coef07);
		_EJ Vector<ScalarType, 4> Fac2(Coef08, Coef08, Coef10, Coef11);
		_EJ Vector<ScalarType, 4> Fac3(Coef12, Coef12, Coef14, Coef15);
		_EJ Vector<ScalarType, 4> Fac4(Coef16, Coef16, Coef18, Coef19);
		_EJ Vector<ScalarType, 4> Fac5(Coef20, Coef20, Coef22, Coef23);
		
		_EJ Vector<ScalarType, 4> Vec0(m_[1][0], m_[0][0], m_[0][0], m_[0][0]);
		_EJ Vector<ScalarType, 4> Vec1(m_[1][1], m_[0][1], m_[0][1], m_[0][1]);
		_EJ Vector<ScalarType, 4> Vec2(m_[1][2], m_[0][2], m_[0][2], m_[0][2]);
		_EJ Vector<ScalarType, 4> Vec3(m_[1][3], m_[0][3], m_[0][3], m_[0][3]);
		
		_EJ Vector<ScalarType, 4> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
		_EJ Vector<ScalarType, 4> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
		_EJ Vector<ScalarType, 4> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
		_EJ Vector<ScalarType, 4> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

		_EJ Vector<ScalarType, 4> SignA(+1, -1, +1, -1);
		_EJ Vector<ScalarType, 4> SignB(-1, +1, -1, +1);
		MatrixType_ Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

		_EJ Vector<ScalarType, 4> Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

		_EJ Vector<ScalarType, 4> Dot0(m_[0] * Row0);
		auto Dot1 = (Dot0[0] + Dot0[1]) + (Dot0[2] + Dot0[3]);

		auto OneOverDeterminant = static_cast<ScalarType>(1) / Dot1;

		return m_ = (Inverse * OneOverDeterminant);
	}
}

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
#define __M_OP_EQ_FUNC(op, opFunc) \
	__M_OP_FUNC_TEMPLATE(op) \
	constexpr const _ThisType& operator op##= (const ColumnMatrix<OtherScalarType_, _R, _C>& obj_) { \
		_STD transform(_ImplType::cbegin(), _ImplType::cend(), obj_.cbegin(), _ImplType::begin(), opFunc); \
		return *this; \
	}
#define __M_OP_FUNC(op, type) \
	__M_OP_FUNC_TEMPLATE(op) \
	constexpr const _ThisType& operator op (type obj_) const { \
		return const_cast<_ThisType&>(_ThisType(*this) op##= obj_); \
	}

template <typename _ScalarType, _STD size_t _R, _STD size_t _C>
class ColumnMatrix : private StaticArray<Vector<_ScalarType, _R>, _C> {
	static_assert(_R >= 1, "[EJ][Matrix][Error]: Matrix Row Can't Be Zero");
	static_assert(_C >= 1, "[EJ][Matrix][Error]: Matrix Column Can't Be Zero");
public:
	using ScalarType = _ScalarType;
	using SizeType = _STD size_t;
	using ColumnType = Vector<ScalarType, _R>;
	using RowType = Vector<ScalarType, _C>;

private:
	using _ImplType = StaticArray<Vector<_ScalarType, _R>, _C>;
	using _ThisType = ColumnMatrix<ScalarType, _R, _C>;
	template <typename __T>
	using _ColumnTypeS = Vector<__T, _R>;
	template <typename __T>
	using _RowTypeS = Vector<__T, _C>;

public:
	constexpr ColumnMatrix() : _ImplType() {}
	template<typename OtherScalarType_, typename... OtherScalarTypes_,
		typename = typename _STD enable_if_t<_EJ are_convertible_v<ScalarType, OtherScalarType_, OtherScalarTypes_...>>>
	constexpr ColumnMatrix(OtherScalarType_ v_, OtherScalarTypes_... vs_) {
		static_assert(false, "[EJ][Matrix][Error]: Invalid Constructor Call. Use Mat(Vec(), Vec(), ...) to construct a matrix with correspond scalar");
	}
	template <typename VectorType_, typename... VectorTypes_,
		typename = _STD enable_if_t<are_convertible_v<ColumnType, VectorType_, VectorTypes_...>>>
	constexpr ColumnMatrix(VectorType_&& vector_, VectorTypes_&&... vectors_) :
		_ImplType{ _STD forward<VectorType_>(vector_), _STD forward<VectorTypes_>(vectors_)... } {}

	template <typename OtherScalarType_, SizeType OtherR_, SizeType OtherC_,
		typename = _STD enable_if_t<_STD is_convertible_v<OtherScalarType_, ScalarType>>>
	constexpr explicit ColumnMatrix(const ColumnMatrix<OtherScalarType_, OtherR_, OtherC_>& obj_) {
		this->operator=(obj_);
	}

	constexpr ColumnMatrix(const _ThisType& obj_) = default;
	constexpr ColumnMatrix(_ThisType&& obj_) noexcept = default;

	template <typename OtherScalarType_, SizeType OtherR_, SizeType OtherC_,
		typename = _STD enable_if<_STD is_convertible_v<OtherScalarType_, ScalarType>>>
	constexpr _ThisType& operator=(const ColumnMatrix<OtherScalarType_, OtherR_, OtherC_>& obj_) {
		if constexpr (OtherC_ <= _C) {
			_STD transform(obj_.cbegin(), obj_.cend(), _ImplType::begin(),
				[](Vector<OtherScalarType_, OtherR_> v_) {
					return static_cast<ColumnType>(v_);
				});
			//_STD fill(_ImplType::begin() + OtherDim_, _ImplType::end(), 0);
		}
		else
			_STD transform(obj_.cbegin(), obj_.cbegin() + _C, _ImplType::begin(),
				[](Vector<OtherScalarType_, OtherR_> v_) {
					return static_cast<ColumnType>(v_);
				});
		return *this;
	}
	_ThisType& operator=(const _ThisType& obj_) noexcept = default;
	_ThisType& operator=(_ThisType&& obj_) noexcept = default;

	~ColumnMatrix() = default;

	using _ImplType::swap;

	constexpr ScalarType* data() noexcept {
		return const_cast<ScalarType*>(const_cast<const _ThisType*>(this)->data());
	}

	constexpr const ScalarType* data() const noexcept {
		return _ImplType::front().data();
	}

	EJ_INLINE _STD string toStringNoHeader() const noexcept;
	EJ_INLINE _STD string toString() const noexcept;

	template <typename ToScalarType_, SizeType ToR_ = _R, SizeType ToC_ = _C>
	ColumnMatrix<ToScalarType_, ToR_, ToC_> to() {
		return ColumnMatrix<ToScalarType_, ToR_, ToC_>(*this);
	}

	using _ImplType::operator[];
	constexpr auto operator==(const _ThisType& obj_) const {
		return static_cast<_ImplType>(*this) == static_cast<_ImplType>(obj_);
	}
	constexpr auto operator<=>(const _ThisType& obj_) const {
		return static_cast<_ImplType>(*this) <=> static_cast<_ImplType>(obj_);
	}
	__M_OP_FUNC_TEMPLATE(*)
	constexpr const ColumnType operator*(const _RowTypeS<OtherScalarType_>& obj_) const {
		return _STD transform_reduce(_ImplType::cbegin(), _ImplType::cend(), obj_.cbegin(), obj_.cend(),
			_ImplType::begin(), _STD multiplies{});
	}
	__M_OP_FUNC(*, const _RowTypeS<OtherScalarType_>&);
	template <typename OtherScalarType_, SizeType OtherC_,
		typename = _STD enable_if_t<_STD is_convertible_v<
		decltype(_STD declval<ScalarType>() * _STD declval<OtherScalarType_>()), ScalarType>>>
	constexpr const ColumnMatrix<ScalarType, _R, OtherC_>
		operator*(const ColumnMatrix<OtherScalarType_, _C, OtherC_>& obj_) const {
		ColumnMatrix<ScalarType, _R, OtherC_> temp{};
		_STD transform(_ImplType::cbegin(), _ImplType::cend(), obj_.cbegin(), temp.begin(),
			_STD multiplies{});
		return temp;
	}
	__M_OP_SCALAR_EQ_FUNC(*, _STD multiplies{});
	__M_OP_SCALAR_EQ_FUNC(/, _STD divides{});
	__M_OP_FUNC(*, const OtherScalarType_&);
	__M_OP_FUNC(/, const OtherScalarType_&);
	__M_OP_EQ_FUNC(+, _STD plus{});
	__M_OP_EQ_FUNC(-, _STD minus{});
#define __OTHER_MATRIX_TYPE ColumnMatrix<OtherScalarType_, _R, _C>
	__M_OP_FUNC(+, const __OTHER_MATRIX_TYPE&);
	__M_OP_FUNC(-, const __OTHER_MATRIX_TYPE&);
#undef __OTHER_MATRIX_TYPE
	constexpr _ThisType operator-() const {
		_ThisType temp{};
		_STD transform(_ImplType::cbegin(), _ImplType::cend(), temp.begin(), _STD negate<>());
		return temp;
	}

	template <typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>>
	constexpr auto determinate() const;

	template <typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>>
	constexpr auto trace() const;

	template <typename Type_>
	constexpr bool isInvertible(Type_ det_) const {
		if constexpr (!is_square_matrix_v<_ThisType>) {
			return false;
		}
		return det_ != 0;
	}
	constexpr bool isInvertible() const {
		if constexpr (!is_square_matrix_v<_ThisType>) {
			return false;
		}
		return determinate() != 0;
	}

	template <typename Type_, 
		typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>>
	constexpr const _ThisType& inverse(Type_ det_);
	template <typename Type_,
		typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>>
	constexpr const _ThisType inversed(Type_ det_) const {
		return _ThisType(*this).inverse(det_);
	}
	template <typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>>
	constexpr const _ThisType& inverse() {
		if constexpr (_R == 4) {
			return ::_inverseImpl44(*this);
		}
		return inverse(determinate());
	}
	template <typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>>
	constexpr const _ThisType inversed() const {
		return _ThisType(*this).inverse();
	}

	template <typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>>
	constexpr const _ThisType& transpose() {
		ColumnMatrix<ScalarType, _C, _R> temp{};
		for (SizeType i = 0; i < _R; ++i) {
			for (SizeType j = 0; j < _C; ++j) {
				temp[j][i] = (*this)[i][j];
			}
		}
		operator=(temp);
		return *this;
	}
	constexpr const ColumnMatrix<ScalarType, _C, _R> transposed() const {
		ColumnMatrix<ScalarType, _C, _R> temp{};
		for (SizeType i = 0; i < _R; ++i) {
			for (SizeType j = 0; j < _C; ++j) {
				temp[j][i] = (*this)[i][j];
			}
		}
		return temp;
	}

	template <typename Type_,
		typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>,
		typename = _STD enable_if_t<_R == 4>>
	constexpr const _ThisType& translate(const Vector<Type_, 3>& v_) {
		(*this)[3][0] += v_[0];
		(*this)[3][1] += v_[1];
		(*this)[3][2] += v_[2];
		return *this;
	}
	template <typename Type_,
		typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>,
		typename = _STD enable_if_t<_R == 4>>
	constexpr const _ThisType translated(const Vector<Type_, 3>& v_) const {
		return _ThisType(*this).translate(v_);
	}

	template <typename Type_,
		typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>,
		typename = _STD enable_if_t<_R == 4>>
	constexpr const _ThisType& scale(const Vector<Type_, 3>& v_) {
		(*this)[0] *= v_[0];
		(*this)[1] *= v_[1];
		(*this)[2] *= v_[2];
		return *this;
	}
	template <typename Type_,
		typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>,
		typename = _STD enable_if_t<_R == 4>>
	constexpr const _ThisType scaled(const Vector<Type_, 3>& v_) const {
		return _ThisType{ (*this)[0] * v_[0], (*this)[1] * v_[1], (*this)[2] * v_[2], (*this)[3] };
	}

	template <typename AngleType_, typename VScalarType_,
		typename = _STD enable_if_t<_EJ are_convertible_v<ScalarType, AngleType_, VScalarType_>>,
		typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>,
		typename = _STD enable_if_t<_R == 4>>
	constexpr const _ThisType& rotate(const AngleType_ angle_, const Vector<VScalarType_, 3>& axis_) {
		return this->operator=(rotated(angle_, axis_));
	}
	template <typename AngleType_, typename VScalarType_,
		typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>,
		typename = _STD enable_if_t<_R == 4>>
	constexpr const _ThisType rotated(const AngleType_ angle_, const Vector<VScalarType_, 3>& axis_) const;

	using _ImplType::begin;
	using _ImplType::rbegin;
	using _ImplType::cbegin;
	using _ImplType::crbegin;
	using _ImplType::end;
	using _ImplType::rend;
	using _ImplType::cend;
	using _ImplType::crend;

private:

// static
public:
	static constexpr _ThisType identity() {
		if constexpr (_R == 4) {
			return _ThisType{
				ColumnType{1, 0, 0, 0},
				ColumnType{0, 1, 0, 0},
				ColumnType{0, 0, 1, 0},
				ColumnType{0, 0, 0, 1}
			};
		}
		_ThisType temp{};
		for (int i = 0; i < _R; ++i) {
			temp[i][i] = 1;
		}
		return temp;
	}

	template <typename AngleType_, typename VScalarType_,
		typename = _STD enable_if_t<_EJ are_convertible_v<ScalarType, AngleType_, VScalarType_>>,
		typename = _STD enable_if_t<_EJ is_square_matrix_v<_ThisType>>,
		typename = _STD enable_if_t<_R == 3 || _R == 4>>
	static constexpr _ThisType angleAxis(const AngleType_ angle_, const Vector<VScalarType_, 3>& axis_) {
		auto const a = angle_;
		auto const c = _STD cos(a);
		auto const s = _STD sin(a);

		using __T = decltype(_STD declval<AngleType_>()* _STD declval<VScalarType_>());

		auto axis = axis_.normalized();
		Vector<__T, 3> temp(axis * (static_cast<__T>(1) - c));

		ColumnMatrix<__T, 3, 3> Rotate;
		Rotate[0][0] = temp[0] * axis[0] + c;
		Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
		Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

		Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
		Rotate[1][1] = temp[1] * axis[1] + c;
		Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

		Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
		Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
		Rotate[2][2] = temp[2] * axis[2] + c;
		if constexpr (_R == 3) {
			return Rotate;
		}
		else if constexpr (_R == 4) {
			return _ThisType(Vector<ScalarType, 4>(Rotate[0][0], Rotate[0][1], Rotate[0][2]),
							 Vector<ScalarType, 4>(Rotate[1][0], Rotate[1][1], Rotate[1][2]),
							 Vector<ScalarType, 4>(Rotate[2][0], Rotate[2][1], Rotate[2][2]),
							 Vector<ScalarType, 4>(0, 0, 0, 1));
		}
	}

};

#undef __M_OP_FUNC_TEMPLATE
#undef __M_OP_SCALAR_EQ_FUNC
#undef __M_OP_EQ_FUNC
#undef __M_OP_FUNC

template <typename _ScalarType, _STD size_t _R, _STD size_t _C>
_STD string ColumnMatrix<_ScalarType, _R, _C>::toStringNoHeader() const noexcept {
	return _STD accumulate(_ImplType::cbegin() + 1, _ImplType::cend(), _STD string("{\n\t") + _ImplType::front().toStringNoHeader(),
		[](_STD string s_, const ColumnType& v_) {
			return s_ + ",\n\t" + v_.toStringNoHeader();
		}
	) + "\n}";
}
template <typename _ScalarType, _STD size_t _R, _STD size_t _C>
_STD string ColumnMatrix<_ScalarType, _R, _C>::toString() const noexcept {
	return _STD string("[EJ][ColumnMatrix]") + toStringNoHeader();
}

template <typename _ScalarType, _STD size_t _R, _STD size_t _C>
template <typename>
constexpr auto ColumnMatrix<_ScalarType, _R, _C>::determinate() const {
	if constexpr (_R == 2) {
		return (*this)[0][0] * (*this)[1][1] - (*this)[1][0] * (*this)[0][1];
	}
	else if constexpr (_R == 3) {
		return
			+(*this)[0][0] * ((*this)[1][1] * (*this)[2][2] - (*this)[2][1] * (*this)[1][2])
			-(*this)[1][0] * ((*this)[0][1] * (*this)[2][2] - (*this)[2][1] * (*this)[0][2])
			+(*this)[2][0] * ((*this)[0][1] * (*this)[1][2] - (*this)[1][1] * (*this)[0][2]);
	}
	else if constexpr (_R == 4) {
		auto subFactor00 = (*this)[2][2] * (*this)[3][3] - (*this)[3][2] * (*this)[2][3];
		auto subFactor01 = (*this)[2][1] * (*this)[3][3] - (*this)[3][1] * (*this)[2][3];
		auto subFactor02 = (*this)[2][1] * (*this)[3][2] - (*this)[3][1] * (*this)[2][2];
		auto subFactor03 = (*this)[2][0] * (*this)[3][3] - (*this)[3][0] * (*this)[2][3];
		auto subFactor04 = (*this)[2][0] * (*this)[3][2] - (*this)[3][0] * (*this)[2][2];
		auto subFactor05 = (*this)[2][0] * (*this)[3][1] - (*this)[3][0] * (*this)[2][1];

		auto detCof0 = +((*this)[1][1] * subFactor00 - (*this)[1][2] * subFactor01 + (*this)[1][3] * subFactor02);
		auto detCof1 = -((*this)[1][0] * subFactor00 - (*this)[1][2] * subFactor03 + (*this)[1][3] * subFactor04);
		auto detCof2 = +((*this)[1][0] * subFactor01 - (*this)[1][1] * subFactor03 + (*this)[1][3] * subFactor05);
		auto detCof3 = -((*this)[1][0] * subFactor02 - (*this)[1][1] * subFactor04 + (*this)[1][2] * subFactor05);

		return
			(*this)[0][0] * detCof0 + (*this)[0][1] * detCof1 +
			(*this)[0][2] * detCof2 + (*this)[0][3] * detCof3;
	}
	else {
		static_assert(false, "[EJ][Matrix][Error]: Determinate For Matrix Size > 4 Not Implemented Yet");
	}
}

template <typename _ScalarType, _STD size_t _R, _STD size_t _C>
template <typename>
constexpr auto ColumnMatrix<_ScalarType, _R, _C>::trace() const {
	auto sum = (*this)[0][0];
	for (SizeType i = 1; i < _R; ++i) {
		sum += (*this)[i][i];
	}
	return sum;
}

template <typename _ScalarType, _STD size_t _R, _STD size_t _C>
template <typename Type_, typename>
constexpr const ColumnMatrix<_ScalarType, _R, _C>& ColumnMatrix<_ScalarType, _R, _C>::inverse(Type_ det_) {
	if constexpr (_R == 2) {
		auto invDet = 1 / det_;
		auto temp = (*this)[0][0];
		(*this)[0][0] = +(*this)[1][1] * invDet;
		(*this)[1][1] = +temp * invDet;
		(*this)[0][1] = -(*this)[0][1] * invDet;
		(*this)[1][0] = -(*this)[1][0] * invDet;
		return *this;
	}
	else if constexpr (_R == 3) {
		auto invDet = 1 / det_;
		auto temp = (*this)[0][0];

		(*this)[0][0] = +((*this)[1][1] * (*this)[2][2] - (*this)[2][1] * (*this)[1][2]) * invDet;
		(*this)[1][0] = -((*this)[1][0] * (*this)[2][2] - (*this)[2][0] * (*this)[1][2]) * invDet;
		(*this)[2][0] = +((*this)[1][0] * (*this)[2][1] - (*this)[2][0] * (*this)[1][1]) * invDet;
		(*this)[0][1] = -((*this)[0][1] * (*this)[2][2] - (*this)[2][1] * (*this)[0][2]) * invDet;
		(*this)[1][1] = +((*this)[0][0] * (*this)[2][2] - (*this)[2][0] * (*this)[0][2]) * invDet;
		(*this)[2][1] = -((*this)[0][0] * (*this)[2][1] - (*this)[2][0] * (*this)[0][1]) * invDet;
		(*this)[0][2] = +((*this)[0][1] * (*this)[1][2] - (*this)[1][1] * (*this)[0][2]) * invDet;
		(*this)[1][2] = -((*this)[0][0] * (*this)[1][2] - (*this)[1][0] * (*this)[0][2]) * invDet;
		(*this)[2][2] = +((*this)[0][0] * (*this)[1][1] - (*this)[1][0] * (*this)[0][1]) * invDet;
		return *this;
	}
	else if constexpr (_R == 4) {
		return ::_inverseImpl44(*this);
	}
	else {
		static_assert(false, "[EJ][Matrix][Error]: Inverse For Matrix Size > 4 Not Implemented Yet");
	}
}

template <typename _ScalarType, _STD size_t _R, _STD size_t _C>
template <typename AngleType_, typename VScalarType_, typename, typename>
constexpr const ColumnMatrix<_ScalarType, _R, _C>
	ColumnMatrix<_ScalarType, _R, _C>::rotated(const AngleType_ angle_, const Vector<VScalarType_, 3>& axis_) const {
	using __T = decltype(_STD declval<AngleType_>() * _STD declval<VScalarType_>());\

	ColumnMatrix<__T, 3, 3> Rotate = ColumnMatrix<__T, 3, 3>::angleAxis(angle_, axis_);

	ColumnMatrix<_ScalarType, 4, 4> Result;
	Result[0] = (*this)[0] * Rotate[0][0] + (*this)[1] * Rotate[0][1] + (*this)[2] * Rotate[0][2];
	Result[1] = (*this)[0] * Rotate[1][0] + (*this)[1] * Rotate[1][1] + (*this)[2] * Rotate[1][2];
	Result[2] = (*this)[0] * Rotate[2][0] + (*this)[1] * Rotate[2][1] + (*this)[2] * Rotate[2][2];
	Result[3] = (*this)[3];
	return Result;
}

using Mat2i = ColumnMatrix<int, 2, 2>;
using Mat2u = ColumnMatrix<unsigned int, 2, 2>;
using Mat2f = ColumnMatrix<float, 2, 2>;
using Mat2d = ColumnMatrix<double, 2, 2>;
using Mat3i = ColumnMatrix<int, 3, 3>;
using Mat3u = ColumnMatrix<unsigned int, 3, 3>;
using Mat3f = ColumnMatrix<float, 3, 3>;
using Mat3d = ColumnMatrix<double, 3, 3>;
using Mat4i = ColumnMatrix<int, 4, 4>;
using Mat4u = ColumnMatrix<unsigned int, 4, 4>;
using Mat4f = ColumnMatrix<float, 4, 4>;
using Mat4d = ColumnMatrix<double, 4, 4>;

EJ_NAMESPACE_END

/************************************************************************/
/*                             TEST SOURCE                              */
/************************************************************************/
/*

using namespace EJ;
int main() {
#define TEST_AND_LOG(TEST_) \
	std::cout << #TEST_ ":\n" << (TEST_) << '\n'

#define LOG_LINE \
	std::cout << "--------------------------------------------------------------------------------------\n"

	// test constructors
	LOG_LINE;
	std::cout << "Constructors\n";
	LOG_LINE;
	TEST_AND_LOG(Mat4f{});
	TEST_AND_LOG(Mat4f::identity());
	TEST_AND_LOG(Mat4f(Vec4f{ 1, 2, 3, 4 }, Vec4f{ 5, 6, 7, 8 }, Vec4f{ 9, 10, 11, 12 }));
	TEST_AND_LOG(Mat4f(Mat4f(Vec4f{ 1 })));
	TEST_AND_LOG(Mat4f(std::move(Mat4f(Vec4f{ 1 }))));

	Mat4f m1{ Vec4f{ 1, 2, 3, 4 }, Vec4f{ 5, 6, 7, 8 }, Vec4f{ 9, 10, 11, 12 }, Vec4f{ 13, 14, 15, 16 } };
	TEST_AND_LOG(m1);
	Mat4f m2{ -m1 };
	TEST_AND_LOG(m2);
	Mat4f m3{ m1 * 2 };
	TEST_AND_LOG(m3);

	// test assignment operators
	LOG_LINE;
	std::cout << "Assignment operators\n";
	LOG_LINE;
	TEST_AND_LOG(m3 = m1);
	TEST_AND_LOG(m3 = std::move(Mat4f(m2)));
	TEST_AND_LOG(m3 = Mat4f(Vec4f{ 1, 2 }));

	// test scalar operators
	LOG_LINE;
	std::cout << "Scalar operators\n";
	LOG_LINE;
	TEST_AND_LOG(-m1);
	TEST_AND_LOG(m1 * 10.0);
	TEST_AND_LOG(m1 *= 10.0);
	TEST_AND_LOG(m1 / 10.0);
	TEST_AND_LOG(m1 /= 10.0);
	TEST_AND_LOG(m1.translated(Vec3f{ 1, 2, 3 }));
	TEST_AND_LOG(Mat4f(m1).translate(Vec3f{ 1, 2, 3 }));
	TEST_AND_LOG(m1.scaled(Vec3f{ 1, 2, 3 }));
	TEST_AND_LOG(Mat4f(m1).scale(Vec3f{ 1, 2, 3 }));
	TEST_AND_LOG(m1.rotated(3.14159265, Vec3f{ 1, 2, 3 }));
	TEST_AND_LOG(Mat4f(m1).rotate(3.14159265, Vec3f{ 1, 2, 3 }));

	// test Compute operators
	LOG_LINE;
	std::cout << "Compute operators\n";
	LOG_LINE;
	TEST_AND_LOG(m1 + m2);
	TEST_AND_LOG(m1 - m2);
	TEST_AND_LOG(m1 * m2);
	TEST_AND_LOG(m1 += m2);
	TEST_AND_LOG(m1 -= m2);

	// test compare operators
	LOG_LINE;
	std::cout << "Compare operators\n";
	LOG_LINE;
	TEST_AND_LOG(m1 < m2);
	TEST_AND_LOG(m1 == m2);
	TEST_AND_LOG(m1 > m2);

	// test other functions
	LOG_LINE;
	std::cout << "Other functions\n";
	LOG_LINE;
	TEST_AND_LOG(m1.determinate());
	TEST_AND_LOG(m1.trace());
	TEST_AND_LOG(m1.inversed());
	TEST_AND_LOG(m1.inversed(m1.determinate()));
	TEST_AND_LOG(Mat4f(m1).inverse());
	TEST_AND_LOG(Mat4f(m1).inverse(m1.determinate()));
	TEST_AND_LOG(m1.transposed());
	TEST_AND_LOG(Mat4f(m1).transpose());

	// test raw pointer
	LOG_LINE;
	std::cout << "Raw pointer\n";
	LOG_LINE;
	auto p = m1.data();
	for (int i = 0; i < 16; ++i) {
		TEST_AND_LOG(p[i]);
	}
}

*/

#endif // EJ_MATRIX_HPP

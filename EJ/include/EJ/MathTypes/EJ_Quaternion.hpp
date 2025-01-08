#ifndef EJ_QUATERNION_HPP
#define EJ_QUATERNION_HPP

#include "../utils/EJ_config.hpp"
#include "../utils/EJ_Utils.hpp"
#include "../Containers/EJ_StaticArray.hpp"
#include "../MathTypes/EJ_Matrix.hpp"

EJ_NAMESPACE_BEGIN

template <typename _ScalarType>
class Quaternion : private StaticArray<_ScalarType, 4> {
public:
    using ScalarType = _ScalarType;

private:
    using _ImplType = StaticArray<ScalarType, 4>;
    using _ThisType = Quaternion<ScalarType>;

public:
    constexpr Quaternion() : _ImplType{ 1, 0, 0, 0 } {}
    template <typename WType_,
        typename = _STD enable_if_t<_STD is_convertible_v<WType_, ScalarType>>>
    constexpr Quaternion(WType_ w_) :
        _ImplType{ static_cast<ScalarType>(w_), 0, 0, 0 } {}
    template <typename WType_, typename XType_, typename YType_, typename ZType_,
        typename = _STD enable_if_t<_EJ are_convertible_v<ScalarType, WType_, XType_, YType_, ZType_>>>
    constexpr Quaternion(WType_ w_, XType_ x_, YType_ y_, ZType_ z_) :
        _ImplType{ static_cast<ScalarType>(w_), static_cast<ScalarType>(x_),
                   static_cast<ScalarType>(y_), static_cast<ScalarType>(z_) } {}

    template <typename OtherScalarType_,
        typename = _STD enable_if_t<_STD is_convertible_v<OtherScalarType_, ScalarType>>>
    constexpr explicit Quaternion(const Quaternion<OtherScalarType_>& obj_) {
        this->operator=(obj_);
    }

    constexpr Quaternion(const _ThisType& obj_) = default;
    constexpr Quaternion(_ThisType&& obj_) noexcept = default;

	~Quaternion() = default;

    template <typename OtherScalarType_,
        typename = _STD enable_if<_STD is_convertible_v<OtherScalarType_, ScalarType>>>
    constexpr _ThisType& operator=(const Quaternion<OtherScalarType_>& obj_) {
        _STD transform(obj_.cbegin(), obj_.cend(), _ImplType::begin(),
            [](OtherScalarType_ s_) {
                return static_cast<ScalarType>(s_);
            });
        return *this;
    }
    _ThisType& operator=(const _ThisType& obj_) noexcept = default;
    _ThisType& operator=(_ThisType&& obj_) noexcept = default;

    template <typename OtherScalarType_ = ScalarType>
    constexpr explicit operator ColumnMatrix<OtherScalarType_, 3, 3>() const {
        auto qxx((*this)[1] * (*this)[1]);
        auto qyy((*this)[2] * (*this)[2]);
        auto qzz((*this)[3] * (*this)[3]);
        auto qxz((*this)[1] * (*this)[3]);
        auto qxy((*this)[1] * (*this)[2]);
        auto qyz((*this)[2] * (*this)[3]);
        auto qwx((*this)[0] * (*this)[1]);
        auto qwy((*this)[0] * (*this)[2]);
        auto qwz((*this)[0] * (*this)[3]);

		return ColumnMatrix<OtherScalarType_, 3, 3>({
			Vector<OtherScalarType_, 3>{ static_cast<OtherScalarType_>(1) - static_cast<OtherScalarType_>(2) * (qyy + qzz), static_cast<OtherScalarType_>(2) * (qxy + qwz), static_cast<OtherScalarType_>(2) * (qxz - qwy) },
			Vector<OtherScalarType_, 3>{ static_cast<OtherScalarType_>(2) * (qxy - qwz), static_cast<OtherScalarType_>(1) - static_cast<OtherScalarType_>(2) * (qxx + qzz), static_cast<OtherScalarType_>(2) * (qyz + qwx) },
			Vector<OtherScalarType_, 3>{ static_cast<OtherScalarType_>(2) * (qxz + qwy), static_cast<OtherScalarType_>(2) * (qyz - qwx), static_cast<OtherScalarType_>(1) - static_cast<OtherScalarType_>(2) * (qxx + qyy) }
			});
    }
    template <typename OtherScalarType_ = ScalarType>
    constexpr explicit operator ColumnMatrix<OtherScalarType_, 4, 4>() const {
		ColumnMatrix<OtherScalarType_, 4, 4> Result(ColumnMatrix<OtherScalarType_, 3, 3>(*this));
        Result[3][3] = static_cast<OtherScalarType_>(1);
        return Result;
    }

    using _ImplType::swap;

    using _ImplType::data;

    _STD string toStringNoHeader() const noexcept {
        return _EJ toStringNoHeader(static_cast<_ImplType>(*this));
    }
    _STD string toString() const noexcept {
        return _STD string("[EJ][Quaternion]") + toStringNoHeader();
    }

    template <typename ToScalarType_>
    Quaternion<ToScalarType_> to() {
        return Quaternion<ToScalarType_>(*this);
    }

    using _ImplType::operator[];
    constexpr auto operator==(const _ThisType& obj_) const {
        return static_cast<_ImplType>(*this) == static_cast<_ImplType>(obj_);
    }
    constexpr auto operator<=>(const _ThisType& obj_) const {
        return static_cast<_ImplType>(*this) <=> static_cast<_ImplType>(obj_);
    }

    template <typename OtherScalarType_,
        typename = _STD enable_if<_STD is_convertible_v<OtherScalarType_, ScalarType>>>
    const _ThisType& operator*=(const Quaternion<OtherScalarType_>& obj_) const {
        (*this)[0] = (*this)[0] * obj_[0] - (*this)[1] * obj_[1] - (*this)[2] * obj_[2] - (*this)[3] * obj_[3];
        (*this)[1] = (*this)[0] * obj_[1] + (*this)[1] * obj_[0] + (*this)[2] * obj_[3] - (*this)[3] * obj_[2];
        (*this)[2] = (*this)[0] * obj_[2] - (*this)[1] * obj_[3] + (*this)[2] * obj_[0] + (*this)[3] * obj_[1];
        (*this)[3] = (*this)[0] * obj_[3] + (*this)[1] * obj_[2] - (*this)[2] * obj_[1] + (*this)[3] * obj_[0];
    }
    template <typename OtherScalarType_,
        typename = _STD enable_if<_STD is_convertible_v<OtherScalarType_, ScalarType>>>
    const _ThisType operator*(const Quaternion<OtherScalarType_>& obj_) const {
        return Quaternion(
            (*this)[0] * obj_[0] - (*this)[1] * obj_[1] - (*this)[2] * obj_[2] - (*this)[3] * obj_[3],
            (*this)[0] * obj_[1] + (*this)[1] * obj_[0] + (*this)[2] * obj_[3] - (*this)[3] * obj_[2],
            (*this)[0] * obj_[2] - (*this)[1] * obj_[3] + (*this)[2] * obj_[0] + (*this)[3] * obj_[1],
            (*this)[0] * obj_[3] + (*this)[1] * obj_[2] - (*this)[2] * obj_[1] + (*this)[3] * obj_[0]
        );
    }

    constexpr auto sqrNorm() const {
        return (*this)[0] * (*this)[0] + (*this)[1] * (*this)[1] + (*this)[2] * (*this)[2] + (*this)[3] * (*this)[3];
    }
    constexpr auto length() const {
        return _STD sqrt(sqrNorm());
    }

    const _ThisType& conjugate() {
		(*this)[1] = -(*this)[1];
		(*this)[2] = -(*this)[2];
		(*this)[3] = -(*this)[3];
        return *this;
    }
    const _ThisType conjugated() const {
        return Quaternion((*this)[0], -(*this)[1], -(*this)[2], -(*this)[3]);
    }

    const _ThisType& inverse() {
		auto oneOverSqrNorm = static_cast<ScalarType>(1) / sqrNorm();
		(*this)[0] *= oneOverSqrNorm;
		(*this)[1] *= -oneOverSqrNorm;
		(*this)[2] *= -oneOverSqrNorm;
		(*this)[3] *= -oneOverSqrNorm;
        return *this;
    }
    const _ThisType inversed() const {
        return _ThisType(*this).inverse();
    }

    void normalize() {
        float len = length();
        (*this)[0] /= len;
        (*this)[1] /= len;
        (*this)[2] /= len;
        (*this)[3] /= len;
    }
    void normalized() const {
        return _ThisType(*this).normalize();
    }

    Vector<ScalarType, 3> eulerAngles() const {
        return Vector<ScalarType, 3>{ pitch(), yaw(), roll() };
    }

    ScalarType roll() const {
        const auto y = static_cast<ScalarType>(2) * ((*this)[1] * (*this)[2] + (*this)[0] * (*this)[3]);
        const auto x = (*this)[0] * (*this)[0] + (*this)[1] * (*this)[1] - (*this)[2] * (*this)[2] - (*this)[3] * (*this)[3];
        if (_STD abs(x) < std::numeric_limits<decltype(x)>::epsilon && _STD abs(y) < std::numeric_limits<decltype(y)>::epsilon)
            return static_cast<ScalarType>(0);
        return static_cast<ScalarType>(_STD atan(y, x));
    }

    ScalarType pitch() const {
        const auto y = static_cast<ScalarType>(2) * ((*this)[2] * (*this)[3] + (*this)[0] * (*this)[1]);
        const auto x = (*this)[0] * (*this)[0] - (*this)[1] * (*this)[1] - (*this)[2] * (*this)[2] + (*this)[3] * (*this)[3];
        if (_STD abs(x) < std::numeric_limits<decltype(x)>::epsilon && _STD abs(y) < std::numeric_limits<decltype(y)>::epsilon)
            return static_cast<ScalarType>(static_cast<ScalarType>(2) * _STD atan((*this)[1], (*this)[0]));
        return static_cast<ScalarType>(_STD atan(y, x));
    }

    ScalarType yaw() const {
        return _STD asin(_STD clamp(static_cast<ScalarType>(-2) * ((*this)[1] * (*this)[3] - (*this)[0] * (*this)[2]),
            static_cast<ScalarType>(-1), static_cast<ScalarType>(1)));
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
    // [0] => w, [1] => x, [2] => y, [3] => z

// static
public:
    template <typename OtherScalarType_,
        typename = _STD enable_if<_STD is_convertible_v<OtherScalarType_, ScalarType>>>
	static Quaternion<ScalarType> fromEulerAngles(const Vector<OtherScalarType_, 3>& angles_) {
        return fromEulerAngles(angles_[0], angles_[1], angles_[2]);
	}

    template <typename PType_, typename YType_, typename RType_,
        typename = _STD enable_if_t<_EJ are_convertible_v<ScalarType, PType_, YType_, RType_>>>
	static Quaternion<ScalarType> fromEulerAngles(PType_ pitch_, YType_ yaw_, RType_ roll_) {
		auto c1 = _STD cos(pitch_ / static_cast<ScalarType>(2));
		auto c2 = _STD cos(  yaw_ / static_cast<ScalarType>(2));
		auto c3 = _STD cos( roll_ / static_cast<ScalarType>(2));
		auto s1 = _STD sin(pitch_ / static_cast<ScalarType>(2));
		auto s2 = _STD sin(  yaw_ / static_cast<ScalarType>(2));
		auto s3 = _STD sin( roll_ / static_cast<ScalarType>(2));
		return Quaternion<ScalarType>(
			c1 * c2 * c3 - s1 * s2 * s3,
			s1 * c2 * c3 + c1 * s2 * s3,
			c1 * s2 * c3 - s1 * c2 * s3,
			c1 * c2 * s3 + s1 * s2 * c3
		);
	}

    template <typename AngleType_, typename VScalarType_,
        typename = _STD enable_if_t<_EJ are_convertible_v<ScalarType, AngleType_, VScalarType_>>>
	static Quaternion<ScalarType> angleAxis(AngleType_ angle_, const Vector<VScalarType_, 3>& axis_) {
		auto halfAngle = angle_ / static_cast<ScalarType>(2);
		auto s = _STD sin(halfAngle);
		return Quaternion<ScalarType>(_STD cos(halfAngle), axis_[0] * s, axis_[1] * s, axis_[2] * s);
	}

    template <typename OtherScalarType_, _STD size_t N_,
        typename = _STD enable_if<_STD is_convertible_v<OtherScalarType_, ScalarType>>,
        typename = _STD enable_if_t<N_ >= 3>>
    static Quaternion<ScalarType> fromMatrix(const ColumnMatrix<OtherScalarType_, N_, N_>& mat_) {
        auto fourXSquaredMinus1 = mat_[0][0] - mat_[1][1] - mat_[2][2];
        auto fourYSquaredMinus1 = mat_[1][1] - mat_[0][0] - mat_[2][2];
        auto fourZSquaredMinus1 = mat_[2][2] - mat_[0][0] - mat_[1][1];
        auto fourWSquaredMinus1 = mat_[0][0] + mat_[1][1] + mat_[2][2];

        int biggestIndex = 0;
        auto fourBiggestSquaredMinus1 = fourWSquaredMinus1;
        if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
            fourBiggestSquaredMinus1 = fourXSquaredMinus1;
            biggestIndex = 1;
        }
        if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
            fourBiggestSquaredMinus1 = fourYSquaredMinus1;
            biggestIndex = 2;
        }
        if (fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
            fourBiggestSquaredMinus1 = fourZSquaredMinus1;
            biggestIndex = 3;
        }
        auto biggestVal = _STD sqrt(fourBiggestSquaredMinus1 + static_cast<OtherScalarType_>(1)) * static_cast<OtherScalarType_>(0.5);
        auto mult = static_cast<OtherScalarType_>(0.25) / biggestVal;

        switch (biggestIndex)
        {
        case 0:
            return _ThisType{ biggestVal, (mat_[1][2] - mat_[2][1]) * mult, (mat_[2][0] - mat_[0][2]) * mult, (mat_[0][1] - mat_[1][0]) * mult };
        case 1:
            return _ThisType{ (mat_[1][2] - mat_[2][1]) * mult, biggestVal, (mat_[0][1] + mat_[1][0]) * mult, (mat_[2][0] + mat_[0][2]) * mult };
        case 2:
            return _ThisType{ (mat_[2][0] - mat_[0][2]) * mult, (mat_[0][1] + mat_[1][0]) * mult, biggestVal, (mat_[1][2] + mat_[2][1]) * mult };
        case 3:
            return _ThisType{ (mat_[0][1] - mat_[1][0]) * mult, (mat_[2][0] + mat_[0][2]) * mult, (mat_[1][2] + mat_[2][1]) * mult, biggestVal };
        }
    }

    template<typename VScalarType1_, typename VScalarType2_,
        typename = _STD enable_if_t<_EJ are_convertible_v<ScalarType, VScalarType1_, VScalarType2_>>>
    static _ThisType lookAt(const Vector<VScalarType1_, 3>& dir_, const Vector<VScalarType2_, 3>& up_)
    {
		using _MatrixType = decltype(_STD declval<VScalarType1_>() * _STD declval<VScalarType2_>());
        ColumnMatrix<_MatrixType, 3, 3> Result;

        Result[2] = -dir_;
        auto Right = up_.cross(Result[2]);
        auto tmp = Right.dot(Right);
#define ___M_MAX(a, b) ((a) > (b) ? (a) : (b))
        Result[0] = Right * (static_cast<ScalarType>(1) / _STD sqrt(___M_MAX(0.00001, tmp)));
#undef ___M_MAX
        Result[1] = Result[2].cross(Result[0]);
        
		return fromMatrix(Result);
    }

};

using Quat = Quaternion<float>;

EJ_NAMESPACE_END

/************************************************************************/
/*                             TEST SOURCE                              */
/************************************************************************/
/*

using namespace EJ;
int main() {
#define TEST_AND_LOG(TEST_) \
	std::cout << std::setw(60) << #TEST_ ": " << (TEST_) << '\n'

#define LOG_LINE \
	std::cout << "--------------------------------------------------------------------------------------\n"

    // test constructors
    LOG_LINE;
    std::cout << "Constructors\n";
    LOG_LINE;
    TEST_AND_LOG(Quat{});
    TEST_AND_LOG(Quat::fromEulerAngles(0, degToRad(90.f), 0));
    TEST_AND_LOG(Quat::fromEulerAngles(Vec3f{ 0, degToRad(90.f), 0 }));
    TEST_AND_LOG(Quat::angleAxis(degToRad(90.f), Vec3f{ 0, 1, 0 }));
    TEST_AND_LOG(Quat::fromMatrix(Mat3f::identity()));
    TEST_AND_LOG(Quat::fromMatrix(Mat4f::angleAxis(degToRad(90.f), Vec3f(0, 1, 0))));
    TEST_AND_LOG(Quat::lookAt(Vec3f(-1, 0, 0), Vec3f(0, 1, 0)));
    TEST_AND_LOG(Quat());
    TEST_AND_LOG(Quat(1));
    TEST_AND_LOG(Quat(1, 2, 3, 4));
    TEST_AND_LOG(Quat(Quat(1, 2, 3, 4)));
    TEST_AND_LOG(Quat(std::move(Quat(1, 2, 3, 4))));

    Quat q1{};
    TEST_AND_LOG(q1);
    Quat q2 = Quat::lookAt(Vec3f{ -1, 0, 0 }, Vec3f{ 0, 1, 0 });
    TEST_AND_LOG(q2);
    Quat q3{ 1, 2, 3, 4 };
    TEST_AND_LOG(q3);

    // test assignment operators
    LOG_LINE;
    std::cout << "Assignment operators\n";
    LOG_LINE;
    TEST_AND_LOG(q3 = q1);
    TEST_AND_LOG(q3 = std::move(Quat(q2)));
    TEST_AND_LOG(q3 = Quat(4));

    //// test scalar operators
    //LOG_LINE;
    //std::cout << "Scalar operators\n";
    //LOG_LINE;
    //TEST_AND_LOG(-m1);
    //TEST_AND_LOG(m1 * 10.0);
    //TEST_AND_LOG(m1 *= 10.0);
    //TEST_AND_LOG(m1 / 10.0);
    //TEST_AND_LOG(m1 /= 10.0);
    //TEST_AND_LOG(m1.translated(Vec3f{ 1, 2, 3 }));
    //TEST_AND_LOG(Mat4f(m1).translate(Vec3f{ 1, 2, 3 }));
    //TEST_AND_LOG(m1.scaled(Vec3f{ 1, 2, 3 }));
    //TEST_AND_LOG(Mat4f(m1).scale(Vec3f{ 1, 2, 3 }));
    //TEST_AND_LOG(m1.rotated(3.14159265, Vec3f{ 1, 2, 3 }));
    //TEST_AND_LOG(Mat4f(m1).rotate(3.14159265, Vec3f{ 1, 2, 3 }));

    //// test Compute operators
    //LOG_LINE;
    //std::cout << "Compute operators\n";
    //LOG_LINE;
    //TEST_AND_LOG(m1 + m2);
    //TEST_AND_LOG(m1 - m2);
    //TEST_AND_LOG(m1 * m2);
    //TEST_AND_LOG(m1 += m2);
    //TEST_AND_LOG(m1 -= m2);

    //// test compare operators
    //LOG_LINE;
    //std::cout << "Compare operators\n";
    //LOG_LINE;
    //TEST_AND_LOG(m1 < m2);
    //TEST_AND_LOG(m1 == m2);
    //TEST_AND_LOG(m1 > m2);

    //// test other functions
    //LOG_LINE;
    //std::cout << "Other functions\n";
    //LOG_LINE;
    //TEST_AND_LOG(m1.determinate());
    //TEST_AND_LOG(m1.trace());
    //TEST_AND_LOG(m1.inversed());
    //TEST_AND_LOG(m1.inversed(m1.determinate()));
    //TEST_AND_LOG(Mat4f(m1).inverse());
    //TEST_AND_LOG(Mat4f(m1).inverse(m1.determinate()));
    //TEST_AND_LOG(m1.transposed());
    //TEST_AND_LOG(Mat4f(m1).transpose());

    // test raw pointer
    LOG_LINE;
    std::cout << "Raw pointer\n";
    LOG_LINE;
    auto p = q1.data();
    TEST_AND_LOG(p[0]);
    TEST_AND_LOG(p[1]);
    TEST_AND_LOG(p[2]);
    TEST_AND_LOG(p[3]);
}

*/

#endif // EJ_QUATERNION_HPP
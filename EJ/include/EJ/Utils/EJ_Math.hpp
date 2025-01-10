#ifndef EJ_TMPMATH_HPP
#define EJ_TMPMATH_HPP

#include "EJ_config.hpp"

EJ_NAMESPACE_BEGIN

template <typename T_ = double>
constexpr static T_ PI = 3.14159265358979323846L;

template <typename T_>
constexpr auto degToRad(T_ deg_) {
	return deg_ * (PI<T_> / static_cast<T_>(180));
}

template <typename T_>
constexpr auto radToDeg(T_ rad_) {
	return rad_ * (static_cast<T_>(180) / PI<T_>);
}

template <typename _T = void>
struct Square {
	constexpr auto operator()(const _T& v_) -> decltype(_STD declval<_T>()* _STD declval<_T>()) const {
		return v_ * v_;
	}
};
template <>
struct Square<void> {
    template <class T_>
    _NODISCARD constexpr auto operator()(T_&& v_) const
        noexcept(noexcept(static_cast<T_&&>(v_) * static_cast<T_&&>(v_)))
        -> decltype(static_cast<T_&&>(v_)* static_cast<T_&&>(v_)) {
        return static_cast<T_&&>(v_) * static_cast<T_&&>(v_);
    }
};

//template <typename T_, _STD size_t S_>
//struct Power {
//	constexpr auto operator()(T_ v_) -> decltype(_STD declval<T_>()* _STD declval<T_>()) {
//		if constexpr (S_ == 0)
//			return 1;
//		else if constexpr (S_ == 1) {
//			return v_;
//		}
//		else if constexpr (S_ & 1) {
//			return Square<T_>{}(Power<T_, S_ / 2>{}(v_)) * v_;
//		}
//		else {
//			return Square<T_>{}(Power<T_, S_ / 2>{}(v_));
//		}
//	}
//};

EJ_NAMESPACE_END

#endif // EJ_TMPMATH_HPP

#ifndef EJ_UTILS_HPP
#define EJ_UTILS_HPP

#include "EJ_config.hpp"

#include "../Utils/EJ_Traits.hpp"

#include <string>

EJ_NAMESPACE_BEGIN

constexpr static _STD string_view EJHeader = "[EJ]";

template <typename T1_, typename T2_>
constexpr EJ_INLINE void swap(T1_& v1_, T2_& v2_) {
	_STD swap(v1_, v2_);
}

template <typename T_>
typename _STD enable_if_t<_STD is_integral_v<T_>, _STD string>
constexpr toString(T_ v_) {
    return _STD to_string(v_);
}

template <typename T_>
typename _STD enable_if_t<_STD is_floating_point_v<T_>, _STD string>
constexpr toString(T_ v_) {
    const auto len = static_cast<size_t>(::_scprintf("%4.2f", v_));
    _STD string s(len, '\0');
    ::sprintf_s(&s[0], len + 1, "%4.2f", v_);
    return s;
}

template <typename T_>
typename _STD enable_if_t<has_func_toString_v<T_>, _STD string>
constexpr toString(const T_& v_) {
    return v_.toString();
}

template <typename Type_>
typename _STD enable_if_t<toString_callable_v<Type_>, _STD ostream&>
operator<<(_STD ostream& os_, const Type_& obj_) {
    os_ << toString(obj_);
    return os_;
}

EJ_NAMESPACE_END

#endif // EJ_UTILS_HPP

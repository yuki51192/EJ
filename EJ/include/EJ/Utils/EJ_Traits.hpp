#ifndef EJ_TRAITS_HPP
#define EJ_TRAITS_HPP

#include "EJ_config.hpp"

#include <type_traits>

EJ_NAMESPACE_BEGIN


template <typename To_, typename... From_>
struct are_convertible : _STD bool_constant<_STD conjunction_v<_STD is_convertible<From_, To_>...>> {};
template <typename To_, typename... From_>
constexpr bool are_convertible_v = are_convertible<To_, From_...>::value;

#define __M_HAS_FUNC_TRAIT(Name_) \
template <typename T_, typename = void> \
struct has_func_##Name_ : _STD false_type {}; \
template <typename T_> \
struct has_func_##Name_ <T_, _STD void_t<decltype(_STD declval<T_>(). Name_ ())>> : _STD true_type {}; \
template <typename T_> \
constexpr bool has_func_##Name_##_v = has_func_##Name_ <T_>::value;

__M_HAS_FUNC_TRAIT(toString)

#undef __M_HAS_FUNC_TRAIT

#define __M_FUNC_CALLABLE_TRAIT(Name_) \
template <typename T_, typename = void> \
struct Name_##_callable : _STD false_type {}; \
template <typename T_> \
struct Name_##_callable<T_, _STD void_t<decltype(Name_ (_STD declval<T_>()))>> : _STD true_type {}; \
template <typename T_> \
constexpr bool Name_##_callable_v = Name_##_callable<T_>::value;

__M_FUNC_CALLABLE_TRAIT(toString)

#undef __M_FUNC_CALLABLE_WITH

EJ_NAMESPACE_END

#endif // EJ_TRAITS_HPP

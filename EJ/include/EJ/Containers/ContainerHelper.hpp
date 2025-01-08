#ifndef EJ_CONTAINERHELPER_HPP
#define EJ_CONTAINERHELPER_HPP

#include "../Utils/EJ_Config.hpp"

#include <array>
#include <vector>
#include <numeric>

EJ_NAMESPACE_BEGIN

template <typename T_, _STD size_t N_>
constexpr _STD string toStringNoHeader(const _STD array<T_, N_>& arr_) {
    return _STD accumulate(arr_.cbegin() + 1, arr_.cend(), _STD string("{ ") + _EJ toString(arr_.front()),
        [](const _STD string& s_, const T_& v_) {
            return s_ + ", " + _EJ toString(v_);
        }
    ) + " }";
}
template <typename T_, _STD size_t N_>
constexpr _STD string toString(const _STD array<T_, N_>& arr_) {
    return _STD string("[EJ][StaticArray]") + toStringNoHeader(arr_);
}

template <typename T_>
constexpr _STD string toStringNoHeader(const _STD vector<T_>& arr_) {
    return _STD accumulate(arr_.cbegin() + 1, arr_.cend(), std::string("{ ") + _EJ toString(arr_.front()),
        [](const std::string& s_, const T_& v_) {
            return s_ + ", " + _EJ toString(v_);
        }
    ) + " }";
}
template <typename T_>
constexpr _STD string toString(const _STD vector<T_>& arr_) {
    return _STD string("[EJ][DynamicArray]") + toStringNoHeader(arr_);
}

EJ_NAMESPACE_END

#endif // EJ_CONTAINERHELPER_HPP

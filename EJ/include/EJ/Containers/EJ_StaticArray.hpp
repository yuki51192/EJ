#ifndef EJ_STATICARRAY_HPP
#define EJ_STATICARRAY_HPP

#include "../utils/EJ_Config.hpp"
#include "../utils/EJ_Utils.hpp"

#include <array>
#include <numeric>

EJ_NAMESPACE_BEGIN

template <typename _Type, _STD size_t _Size>
using StaticArray = _STD array<_Type, _Size>;

template <typename T_, _STD size_t N_>
constexpr ::std::string toStringNoHeader(const StaticArray<T_, N_>& arr_) {
    return ::std::accumulate(arr_.cbegin() + 1, arr_.cend(), ::std::string("{ ") + _EJ toString(arr_.front()),
        [](const ::std::string& s_, const T_& v_) {
            return s_ + ", " + _EJ toString(v_);
        }
    ) + " }";
}
template <typename T_, _STD size_t N_>
constexpr ::std::string toString(const StaticArray<T_, N_>& arr_) {
    return ::std::string("[EJ][StaticArray]") + toStringNoHeader(arr_);
}

EJ_NAMESPACE_END

#endif // EJ_STATICARRAY_HPP

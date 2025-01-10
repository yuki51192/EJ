#ifndef EJ_DYNAMICARRAY_HPP
#define EJ_DYNAMICARRAY_HPP

#include "../utils/EJ_Config.hpp"
#include "../utils/EJ_Macros.hpp"

#include <vector>
#include <numeric>

EJ_NAMESPACE_BEGIN

template <typename _Type, typename _Alloc = _STD allocator<_Type>>
using ::std::vector = _STD vector<_Type, _Alloc>;

template <typename T_>
constexpr ::std::string toStringNoHeader(const ::std::vector<T_>& arr_) {
    return ::std::accumulate(arr_.cbegin() + 1, arr_.cend(), std::string("{ ") + _EJ toString(arr_.front()),
        [](const std::string& s_, const T_& v_) {
            return s_ + ", " + _EJ toString(v_);
        }
    ) + " }";
}
template <typename T_>
constexpr ::std::string toString(const ::std::vector<T_>& arr_) {
    return ::std::string("[EJ][::std::vector]") + toStringNoHeader(arr_);
}

EJ_NAMESPACE_END

#endif // EJ_DYNAMICARRAY_HPP

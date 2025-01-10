#ifndef EJ_MATRIXTRAITS_HPP
#define EJ_MATRIXTRAITS_HPP

#include "../utils/EJ_Config.hpp"

EJ_NAMESPACE_BEGIN

template <typename MatrixType_>
struct is_square_matrix : _STD false_type {};
template <template <typename ScalarType_, _STD size_t R_, _STD size_t C_> typename MatrixType_, typename ScalarType_, _STD size_t R_, _STD size_t C_>
struct is_square_matrix<MatrixType_<ScalarType_, R_, C_>> : _STD bool_constant<R_ == C_> {};
template <typename MatrixType_>
constexpr bool is_square_matrix_v = is_square_matrix<MatrixType_>::value;

EJ_NAMESPACE_END

#endif // EJ_MATRIXTRAITS_HPP

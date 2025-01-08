#ifndef EJGL_TRAITS_HPP
#define EJGL_TRAITS_HPP

#include "EJGL_config.hpp"
#include "EJGL_Enum.hpp"

#include <type_traits>

EJGL_NAMESPACE_BEGIN

struct default_create {};

template <DataType _Enum>
struct enumToType {
	using type = void;
};
template <>
struct enumToType<DataType::BYTE> {
	using type = char;
};
template <>
struct enumToType<DataType::UNSIGNED_BYTE> {
	using type = unsigned char;
};
template <>
struct enumToType<DataType::SHORT> {
	using type = short;
};
template <>
struct enumToType<DataType::UNSIGNED_SHORT> {
	using type = unsigned short;
};
template <>
struct enumToType<DataType::INT> {
	using type = int;
};
template <>
struct enumToType<DataType::UNSIGNED_INT> {
	using type = unsigned int;
};
template <>
struct enumToType<DataType::FLOAT> {
	using type = float;
};
template <>
struct enumToType<DataType::DOUBLE> {
	using type = double;
};
template <DataType _Enum>
using enumToType_t = enumToType<_Enum>::type;

template <typename _Type>
struct typeToEnum : _STD integral_constant<DataType, DataType::UNKNOWN> {};
template <>
struct typeToEnum<GLbyte> : _STD integral_constant<DataType, DataType::BYTE> {};
template <>
struct typeToEnum<GLubyte> : _STD integral_constant<DataType, DataType::UNSIGNED_BYTE> {};
template <>
struct typeToEnum<GLshort> : _STD integral_constant<DataType, DataType::SHORT> {};
template <>
struct typeToEnum<GLushort> : _STD integral_constant<DataType, DataType::UNSIGNED_SHORT> {};
template <>
struct typeToEnum<GLint> : _STD integral_constant<DataType, DataType::INT> {};
template <>
struct typeToEnum<GLuint> : _STD integral_constant<DataType, DataType::UNSIGNED_INT> {};
template <>
struct typeToEnum<GLfloat> : _STD integral_constant<DataType, DataType::FLOAT> {};
template <>
struct typeToEnum<GLdouble> : _STD integral_constant<DataType, DataType::DOUBLE> {};

EJGL_NAMESPACE_END

#endif // EJ_TRAITS_HPP

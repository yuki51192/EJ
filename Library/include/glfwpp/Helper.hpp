#ifndef GLFWWRAPPER_HELPER_HPP
#define GLFWWRAPPER_HELPER_HPP

#include <type_traits>

#define GLFWWRAPPER_ENUM_BITS_OPERATORS(Enum)                                                                   \
    inline std::underlying_type_t<Enum> operator~(Enum lhs)                                                     \
    {                                                                                                           \
        return ~static_cast<std::underlying_type_t<Enum>>(lhs);                                                 \
    }                                                                                                           \
    inline std::underlying_type_t<Enum> operator&(Enum lhs, Enum rhs)                                           \
    {                                                                                                           \
        return static_cast<std::underlying_type_t<Enum>>(lhs) & static_cast<std::underlying_type_t<Enum>>(rhs); \
    }                                                                                                           \
    inline std::underlying_type_t<Enum> operator|(Enum lhs, Enum rhs)                                           \
    {                                                                                                           \
        return static_cast<std::underlying_type_t<Enum>>(lhs) | static_cast<std::underlying_type_t<Enum>>(rhs); \
    }                                                                                                           \
    inline std::underlying_type_t<Enum> operator^(Enum lhs, Enum rhs)                                           \
    {                                                                                                           \
        return static_cast<std::underlying_type_t<Enum>>(lhs) ^ static_cast<std::underlying_type_t<Enum>>(rhs); \
    }

#endif  //GLFWWRAPPER_HELPER_HPP
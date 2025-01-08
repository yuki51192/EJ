#ifndef EJGL_CONFIG_HPP
#define EJGL_CONFIG_HPP

#include "../../Utils/EJ_Config.hpp"

// include opengl header here
#include <glad/glad.h>
#include <iostream>

#define EJGL_NAMESPACE_BEGIN EJ_NAMESPACE_BEGIN
#define EJGL_NAMESPACE_END EJ_NAMESPACE_END
#define _EJGL _EJ

// -------------------------------------------------
// options
// -------------------------------------------------
// if you want enum to be enum class, change this to "#define ENUM_STRONG_TYPE_DECLAR class"
#define EJGL_ENUM_STRONG_TYPE_DECLAR 

//
// define this to keep source code of shader
//
#define EJGL_KEEP_SHADER_SRC

//
// define this to that shader program save uniform variable location for better performance
//
#define EJGL_KEEP_UNIFORM_LOCATION

#endif
#ifndef EJGL_SHADER_HPP
#define EJGL_SHADER_HPP

#include "../Utils/EJGL_config.hpp"
#include "../Utils/EJGL_enum.hpp"

#include <memory>
#include <string>
#include <filesystem>

EJGL_NAMESPACE_BEGIN

class Shader
{
private:
	class _ShaderImpl;

public:
	// construct empty
	Shader();
	// construct with src
	Shader(_STD string_view shaderSrc_, ShaderType shaderType_);
	// construct with id and type
	Shader(GLuint id_, ShaderType type_);
	Shader(const Shader& obj_) = default;
	Shader& operator=(const Shader& obj_) = default;
	~Shader() = default;

	_STD string toStringNoHeader() const noexcept;
	_STD string toString() const noexcept;

	void swap(Shader& obj_) noexcept;

	bool isValid() const;
	GLuint getShaderID() const;
#ifdef EJGL_KEEP_SHADER_SRC
	const _STD string& getShaderSrc() const;
#endif // EJGL_KEEP_SHADER_SRC

	void setShaderID(GLuint id_);
#ifdef EJGL_KEEP_SHADER_SRC
	void setShaderSrc(_STD string_view src_);
#endif // EJGL_KEEP_SHADER_SRC

	bool createShaderByFile(const _STD filesystem::path& filename_, ShaderType shaderType_);
	bool createShaderBySrc(_STD string_view src_, const int length_, ShaderType shaderType_);

	void deleteShader();

private:
	_STD shared_ptr<_ShaderImpl> _impl;

};

EJGL_NAMESPACE_END

#endif // EJGL_SHADER_HPP
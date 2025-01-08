#include "EJ/GL/GlObject/EJGL_Shader.hpp"
#include <fstream>
#include <sstream>

EJGL_NAMESPACE_BEGIN

#pragma region Impl

static GLuint _createShader(const char* src_, const int length_, ShaderType shaderType_) {
	GLuint id = glCreateShader(shaderType_);
	glShaderSource(id, 1, &src_, &length_);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		EJ_ERR_STREAM << "[EJ][Error]Failed to compile " << shaderEnumToStr(shaderType_) << " Shader!: " << message << _STD endl;
		glDeleteShader(id);
		free(message);

		return 0;
	}
	return id;
}

class Shader::_ShaderImpl {
public:
	_ShaderImpl() {}
	_ShaderImpl(_STD string_view shaderSrc_, ShaderType shaderType_) :
		_shaderType(shaderType_)
#ifdef EJGL_KEEP_SHADER_SRC
		, _shaderSrc(shaderSrc_)
#endif // EJGL_KEEP_SHADER_SRC
	{
		_shaderID = _createShader(shaderSrc_.data(), shaderSrc_.length(), shaderType_);
	}
	_ShaderImpl(GLuint id_, ShaderType type_) :
		_shaderID(id_), _shaderType(type_)
	{}
	_ShaderImpl(const _ShaderImpl& obj_) = delete;
	_ShaderImpl(_ShaderImpl&& obj_) noexcept = delete;
	_ShaderImpl& operator=(const _ShaderImpl& obj_) = delete;
	_ShaderImpl& operator=(_ShaderImpl&& obj_) noexcept = delete;
	~_ShaderImpl() {
		deleteShader();
	}

	_STD string toStringNoHeader() const noexcept {
		return _STD string("{ ID: ") + std::to_string(_shaderID) + ", Type: " + shaderEnumToStr(_shaderType) + " }";
	}

	bool createShaderByFile(const _STD filesystem::path& filename_, ShaderType shaderType_) {
		deleteShader();
		
		// read file
		std::ifstream ifs(filename_);
		if (!ifs.is_open()) {
			_shaderID = 0;
			EJ_ERR_STREAM << "[EJ][Error]Cannot open file " << filename_ << std::endl;
			return false;
		}
		std::stringstream ss;
		ss << ifs.rdbuf();

		// creat EJGLShader
		int length = static_cast<int>(ss.str().size());
		_shaderType = shaderType_;
#ifdef EJGL_KEEP_SHADER_SRC
		_shaderSrc = ss.str();
#endif // EJGL_KEEP_SHADER_SRC
		_shaderID = _createShader(ss.str().c_str(), length, shaderType_);
		if (!_shaderID) {
			EJ_ERR_STREAM << "FilePath: " << filename_ << std::endl;
		}
		return _shaderID;
	}
	bool createShaderBySrc(_STD string_view src_, const int length_, ShaderType shaderType_) {
		deleteShader();

		_shaderType = shaderType_;
		_shaderID = _createShader(src_.data(), length_, shaderType_);
#ifdef EJGL_KEEP_SHADER_SRC
		_shaderSrc = src_;
#endif // EJGL_KEEP_SHADER_SRC
		return _shaderID;
	}

	void deleteShader() {
		glDeleteShader(_shaderID);
		_shaderID = 0;
	}

public:
	GLuint _shaderID = 0;
	ShaderType _shaderType;
#ifdef EJGL_KEEP_SHADER_SRC
	_STD string _shaderSrc;
#endif // EJGL_KEEP_SHADER_SRC

};

#pragma endregion

#pragma region Shader

Shader::Shader() :
	_impl{ _STD make_shared<_ShaderImpl>() }
{}
Shader::Shader(_STD string_view shaderSrc_, ShaderType shaderType_) :
	_impl{ _STD make_shared<_ShaderImpl>(shaderSrc_, shaderType_) }
{}
Shader::Shader(GLuint id_, ShaderType type_) :
	_impl{ _STD make_shared<_ShaderImpl>(id_, type_) }
{}

_STD string Shader::toStringNoHeader() const noexcept {
	return _impl->toStringNoHeader();
}
_STD string Shader::toString() const noexcept {
	return _STD string("[EJ][Shader]") + _impl->toStringNoHeader();
}

void Shader::swap(Shader& obj_) noexcept {
	_STD swap(_impl, obj_._impl);
}

bool Shader::isValid() const {
	return _impl->_shaderID != 0;
}
GLuint Shader::getShaderID() const {
	return _impl->_shaderID;
}
#ifdef EJGL_KEEP_SHADER_SRC
const _STD string& Shader::getShaderSrc() const {
	return _impl->_shaderSrc;
}
#endif // EJGL_KEEP_SHADER_SRC

void Shader::setShaderID(GLuint id_) {
	_impl->_shaderID = id_;
}
#ifdef EJGL_KEEP_SHADER_SRC
void Shader::setShaderSrc(_STD string_view src_) {
	_impl->_shaderSrc = src_;
}
#endif // EJGL_KEEP_SHADER_SRC

bool Shader::createShaderByFile(const _STD filesystem::path& filepath_, ShaderType shaderType_) {
	return _impl->createShaderByFile(filepath_, shaderType_);
}
bool Shader::createShaderBySrc(_STD string_view src_, const int length_, ShaderType shaderType_) {
	return _impl->createShaderBySrc(src_, length_, shaderType_);
}

void Shader::deleteShader() {
	return _impl->deleteShader();
}

#pragma endregion

EJGL_NAMESPACE_END
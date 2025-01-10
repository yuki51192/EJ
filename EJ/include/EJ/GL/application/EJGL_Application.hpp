#ifndef EJGL_EJGLAPPLICATION_HPP
#define EJGL_EJGLAPPLICATION_HPP

//
// Tips:
//

#include "../Utils/EJGL_config.hpp"
#include "../../Utils/EJ_Utils.hpp"
#include "../../Utils/EJ_Macros.hpp"

#include <glfwpp/glfw.hpp>

#include <string>

EJGL_NAMESPACE_BEGIN

class Application
{
public:
	Application(const Application& obj_) = delete;
	Application(Application&& obj_) noexcept = delete;

	Application& operator=(const Application& obj_) = delete;
	Application& operator=(Application&& obj_) noexcept = delete;

	virtual ~Application()
	{}

	_STD string toStringNoHeader() const noexcept {
		return _STD string();
	}
	_STD string toString() const noexcept {
		return _STD string("[EJ][Application]");
	}

	EJ_M_CONST_GET_FUNC(const glfw::GlfwLifeHandler&, getGlfwLifeHandler, _glfw);

	// call this after bind context
	void loadGlad() const {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			EJ_ERR_STREAM << "[EJ][GL][ERROR]: Failed to initialize GLAD" << std::endl;
			exit(-1);
		}
	}

protected:
	Application() :
		_glfw{ glfw::init() }
	{}

private:
	glfw::GlfwLifeHandler _glfw;

	// static
public:
	static void initialize() {
		getInstance();
	}
	static Application& getInstance() {
		static Application s_instance;
		return s_instance;
	}

	static glfw::WindowHints getDefaultHint() {
		glfw::WindowHints hints{};
		hints.contextVersionMajor = 4;
		hints.contextVersionMinor = 6;
		hints.openglProfile = glfw::OpenGlProfile::Core;
		return hints;
	}

};

EJGL_NAMESPACE_END

#endif // EJGL_EJGLAPPLICATION_HPP

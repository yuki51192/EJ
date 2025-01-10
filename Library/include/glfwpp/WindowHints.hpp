#ifndef GLFWWRAPPER_WINDOWHINTS_HPP
#define GLFWWRAPPER_WINDOWHINTS_HPP

#include <GLFW/glfw3.h>
#include "Enums.hpp"

namespace glfw
{
    struct WindowHints
    {
        bool resizable = true;
        bool visible = true;
        bool decorated = true;
        bool focused = true;
        bool autoIconify = true;
        bool floating = false;
        bool maximized = false;
        bool centerCursor = true;
        bool transparentFramebuffer = false;
        bool focusOnShow = true;
        bool scaleToMonitor = false;

        int redBits = 8;
        int greenBits = 8;
        int blueBits = 8;
        int alphaBits = 8;
        int depthBits = 24;
        int stencilBits = 8;
        int accumRedBits = 0;
        int accumGreenBits = 0;
        int accumBlueBits = 0;
        int accumAlphaBits = 0;

        int auxBuffers = 0;
        int samples = 0;
        int refreshRate = dontCare;
        bool stereo = false;
        bool srgbCapable = false;
        bool doubleBuffer = true;

        ClientApi clientApi = ClientApi::OpenGl;
        ContextCreationApi contextCreationApi = ContextCreationApi::Native;
        int contextVersionMajor = 1;
        int contextVersionMinor = 0;
        ContextRobustness contextRobustness = ContextRobustness::NoRobustness;
        ContextReleaseBehavior contextReleaseBehavior = ContextReleaseBehavior::Any;
        bool openglForwardCompat = false;
        bool openglDebugContext = false;
        OpenGlProfile openglProfile = OpenGlProfile::Any;

        bool cocoaRetinaFramebuffer = true;
        const char* cocoaFrameName = "";
        bool cocoaGraphicsSwitching = false;

        const char* x11ClassName = "";
        const char* x11InstanceName = "";

        void apply() const
        {
            glfwWindowHint(GLFW_RESIZABLE, resizable);
            glfwWindowHint(GLFW_VISIBLE, visible);
            glfwWindowHint(GLFW_DECORATED, decorated);
            glfwWindowHint(GLFW_FOCUSED, focused);
            glfwWindowHint(GLFW_AUTO_ICONIFY, autoIconify);
            glfwWindowHint(GLFW_FLOATING, floating);
            glfwWindowHint(GLFW_MAXIMIZED, maximized);
            glfwWindowHint(GLFW_CENTER_CURSOR, centerCursor);
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, transparentFramebuffer);
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, focusOnShow);
            glfwWindowHint(GLFW_SCALE_TO_MONITOR, scaleToMonitor);

            glfwWindowHint(GLFW_RED_BITS, redBits);
            glfwWindowHint(GLFW_GREEN_BITS, greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, blueBits);
            glfwWindowHint(GLFW_ALPHA_BITS, alphaBits);
            glfwWindowHint(GLFW_DEPTH_BITS, depthBits);
            glfwWindowHint(GLFW_STENCIL_BITS, stencilBits);
            glfwWindowHint(GLFW_ACCUM_RED_BITS, accumRedBits);
            glfwWindowHint(GLFW_ACCUM_GREEN_BITS, accumGreenBits);
            glfwWindowHint(GLFW_ACCUM_BLUE_BITS, accumBlueBits);
            glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, accumAlphaBits);

            glfwWindowHint(GLFW_AUX_BUFFERS, auxBuffers);
            glfwWindowHint(GLFW_SAMPLES, samples);
            glfwWindowHint(GLFW_REFRESH_RATE, refreshRate);
            glfwWindowHint(GLFW_STEREO, stereo);
            glfwWindowHint(GLFW_SRGB_CAPABLE, srgbCapable);
            glfwWindowHint(GLFW_DOUBLEBUFFER, doubleBuffer);

            glfwWindowHint(GLFW_CLIENT_API, static_cast<int>(clientApi));
            glfwWindowHint(GLFW_CONTEXT_CREATION_API, static_cast<int>(contextCreationApi));
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, contextVersionMajor);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, contextVersionMinor);
            glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, static_cast<int>(contextRobustness));
            glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, static_cast<int>(contextReleaseBehavior));
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, openglForwardCompat);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, openglDebugContext);
            glfwWindowHint(GLFW_OPENGL_PROFILE, static_cast<int>(openglProfile));

            glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, cocoaRetinaFramebuffer);
            glfwWindowHintString(GLFW_COCOA_FRAME_NAME, cocoaFrameName);
            glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, cocoaGraphicsSwitching);

            glfwWindowHintString(GLFW_X11_CLASS_NAME, x11ClassName);
            glfwWindowHintString(GLFW_X11_INSTANCE_NAME, x11InstanceName);
        }
    };
} // namespace glfw

#endif // GLFWWRAPPER_WINDOWHINTS_HPP
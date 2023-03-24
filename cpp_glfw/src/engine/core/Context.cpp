#include "engine/core/Platform.h"

namespace cpp_glfw
{
    ///////////////////////////////////// PUBLIC STATIC API ///////////////////////////////////////

    Window* Context::GetCurrentContext()
    {
        return (Window*)Platform::s_ContextSlot->Get();
    }

    bool Context::StringInExtensionString(const char* string, const char* extensions)
    {
        const char* start = extensions;

        for (;;)
        {
            const char* where = strstr(start, string);
            if (!where)
            {
                return false;
            }

            const char* terminator = where + strlen(string);
            if (where == start
                || *(where - 1) == ' ')
            {
                if (*terminator == ' '
                    || *terminator == '\0')
                {
                    break;
                }
            }

            start = terminator;
        }

        return true;
    }

    const FramebufferConfig* Context::ChooseFramebufferConfig(const FramebufferConfig* desired, const std::vector<FramebufferConfig>& alternatives)
    {
        uint32_t missing, leastMissing = UINT_MAX;
        uint32_t colorDiff, leastColorDiff = UINT_MAX;
        uint32_t extraDiff, leastExtraDiff = UINT_MAX;

        const FramebufferConfig* current;
        const FramebufferConfig* closest = nullptr;

        for (uint32_t i = 0; i < alternatives.size(); i++)
        {
            current = &alternatives[i];

            if (desired->stereo > 0
                && current->stereo == 0)
            {
                //stereo is a hard constraint
                continue;
            }

            if (desired->doubleBuffer != current->doubleBuffer)
            {
                //double buffering is a hard constraint
                continue;
            }

            //count number of missing buffers
            {
                missing = 0;
                if (desired->alphaBits > 0
                    && current->alphaBits == 0)
                {
                    missing++;
                }

                if (desired->depthBits > 0
                    && current->depthBits == 0)
                {
                    missing++;
                }

                if (desired->stencilBits > 0
                    && current->stencilBits == 0)
                {
                    missing++;
                }

                if (desired->auxBuffers > 0
                    && current->auxBuffers < desired->auxBuffers)
                {
                    missing += desired->auxBuffers - current->auxBuffers;
                }

                if (desired->samples > 0
                    && current->samples == 0)
                {
                    //technically, several multisampling buffers could be
                    //involved, but that's a lower level implementation detail
                    //and not important to us here, so we count them as one
                    missing++;
                }

                if (desired->transparent != current->transparent)
                {
                    missing++;
                }
            }

            //these polynomials make many small channel size differences matter
            //less than one large channel size difference

            //calculate color channel size difference value
            {
                colorDiff = 0;

                if (desired->redBits != -1)
                {
                    int32_t diff = desired->redBits - current->redBits;
                    colorDiff += diff * diff;
                }

                if (desired->greenBits != -1)
                {
                    int32_t diff = desired->greenBits - current->greenBits;
                    colorDiff += diff * diff;
                }

                if (desired->blueBits != -1)
                {
                    int32_t diff = desired->blueBits - current->blueBits;
                    colorDiff += diff * diff;
                }
            }

            //calculate non-color channel size difference value
            {
                extraDiff = 0;

                if (desired->alphaBits != -1)
                {
                    int32_t diff = desired->alphaBits - current->alphaBits;
                    extraDiff += diff * diff;
                }

                if (desired->depthBits != -1)
                {
                    int32_t diff = desired->depthBits - current->depthBits;
                    extraDiff += diff * diff;
                }

                if (desired->stencilBits != -1)
                {
                    int32_t diff = desired->stencilBits - current->stencilBits;
                    extraDiff += diff * diff;
                }

                if (desired->accumRedBits != -1)
                {
                    int32_t diff = desired->accumRedBits - current->accumRedBits;
                    extraDiff += diff * diff;
                }

                if (desired->accumGreenBits != -1)
                {
                    int32_t diff = desired->accumGreenBits - current->accumGreenBits;
                    extraDiff += diff * diff;
                }

                if (desired->accumBlueBits != -1)
                {
                    int32_t diff = desired->accumBlueBits - current->accumBlueBits;
                    extraDiff += diff * diff;
                }

                if (desired->accumAlphaBits != -1)
                {
                    int32_t diff = desired->accumAlphaBits - current->accumAlphaBits;
                    extraDiff += diff * diff;
                }

                if (desired->samples != -1)
                {
                    int32_t diff = desired->samples - current->samples;
                    extraDiff += diff * diff;
                }

                if (desired->sRGB
                    && !current->sRGB)
                {
                    extraDiff++;
                }
            }

            //figure out if the current one is better than the best one found so far
            //least number of missing buffers is the most important heuristic,
            //then color buffer size mtch and lastly size match for other buffers

            if (missing < leastMissing)
            {
                closest = current;
            }
            else if (missing == leastMissing)
            {
                if ((colorDiff < leastColorDiff)
                    || (colorDiff == leastColorDiff && extraDiff < leastExtraDiff))
                {
                    closest = current;
                }
            }

            if (current == closest)
            {
                leastMissing = missing;
                leastColorDiff = colorDiff;
                leastExtraDiff = extraDiff;
            }
        }

        return closest;
    }

    bool Context::RefreshContextAttribs(Window* window, const ContextConfig* contextConfig)
    {
        const char* prefixes[] =
        {
            "OpenGL ES-CM ",
            "OpenGL ES-CL ",
            "OpenGL ES ",
            nullptr
        };

        window->m_Context->m_Type = contextConfig->type;
        window->m_Context->m_API = ContextAPI::OpenGL;

        Window* previous = GetCurrentContext();
        MakeContextCurrent(window);

        Context* context = window->m_Context;

        context->GetIntegerv = (PFNGLGETINTEGERVPROC)PlatformGetGLProcAddress("glGetIntegerv");
        context->GetString = (PFNGLGETSTRINGPROC)PlatformGetGLProcAddress("glGetString");
        if (!context->GetIntegerv
            || !context->GetString)
        {
            CPP_GLFW_ERROR("OpenGL GetProcAddress failed!");
            MakeContextCurrent(previous);
            return false;
        }

        const char* version = (const char*)context->GetString(GL_VERSION);
        if (!version)
        {
            if (contextConfig->api == ContextAPI::OpenGL)
            {
                CPP_GLFW_ERROR("OpenGL GetString failed!");
            }
            else
            {
                CPP_GLFW_ERROR("OpenGL ES GetString failed!");
            }
            MakeContextCurrent(previous);
            return false;
        }

        for (int32_t i = 0; prefixes[i]; i++)
        {
            const size_t length = strlen(prefixes[i]);

            if (strncmp(version, prefixes[i], length) == 0)
            {
                version += length;
                context->m_API = ContextAPI::OpenGLES;
                break;
            }
        }

        if (!sscanf(version, "%d.%d.%d",
            &context->m_Major,
            &context->m_Minor,
            &context->m_Revision))
        {
            if (context->m_API == ContextAPI::OpenGL)
            {
                CPP_GLFW_ERROR("No version found on OpenGL version string!");
            }
            else
            {
                CPP_GLFW_ERROR("No version found on OpenGL ES version string!");
            }
            MakeContextCurrent(previous);
            return false;
        }

        if (context->m_Major < contextConfig->major
            || (context->m_Major == contextConfig->major && context->m_Minor < contextConfig->minor))
        {
            //the desired OpenGL version is greater than the actual version
            //this only happens if the machine lacks {GLX|WGL}_ARB_create_context
            //and the user has requested an OpenGL version greater than 1.0

            //for API consistency, we emulate the behavior of the
            //{GLX|WGL}_ARB_create_context extension and fail here

            if (context->m_API == ContextAPI::OpenGL)
            {
                CPP_GLFW_ERROR("Requested OpenGL version %i.%i, got version %i.%i!",
                    contextConfig->major, contextConfig->minor,
                    context->m_Major, context->m_Minor);
            }
            else
            {
                CPP_GLFW_ERROR("Requested OpenGL ES version %i.%i, got version %i.%i!",
                    contextConfig->major, contextConfig->minor,
                    context->m_Major, context->m_Minor);
            }
            MakeContextCurrent(previous);
            return false;
        }

        if (context->m_Major >= 3)
        {
            //OpenGL 3.0+ uses a different function for extension string retrieval
            //we cache it here instead of in ExtensionSupported mostly to alert
            //users as early as possible that their build may be broken

            context->GetStringi = (PFNGLGETSTRINGIPROC)PlatformGetGLProcAddress("glGetStringi");
            if (!context->GetStringi)
            {
                CPP_GLFW_ERROR("Entry point retrieval is broken!");
                MakeContextCurrent(previous);
                return false;
            }
        }

        if (context->m_API == ContextAPI::OpenGL)
        {
            //read back context flags (OpenGL 3.0+)
            if (context->m_Major >= 3)
            {
                GLint flags;
                context->GetIntegerv(GL_CONTEXT_FLAGS, &flags);

                if (flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
                {
                    context->m_Forward = true;
                }

                if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
                {
                    context->m_Debug = true;
                }
                else if (ExtensionSupported("GL_ARB_debug_output")
                    && contextConfig->debug)
                {
                    //HACK: this is a workaround for older drivers (pre KHR_debug)
                    // not setting the debug bit in the context flags for debug contexts
                    context->m_Debug = true;
                }

                if (flags & GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR)
                {
                    context->m_NoError = true;
                }
            }

            //read back OpenGL context profile (OpenGL 3.2+)
            if (context->m_Major >= 4
                || (context->m_Major == 3 && context->m_Minor >= 2))
            {
                GLint mask;
                context->GetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);

                if (mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
                {
                    context->m_Profile = ContextProfile::Compatibility;
                }
                else if (mask & GL_CONTEXT_CORE_PROFILE_BIT)
                {
                    context->m_Profile = ContextProfile::Core;
                }
                else if (ExtensionSupported("GL_ARB_compatibility"))
                {
                    //HACK: this is a workaround for the compatibility profile bit
                    //not being set in the context flags in an OpenGL 3.2+ context
                    //was created without having requested a specific version
                    context->m_Profile = ContextProfile::Compatibility;
                }
            }

            //read back robustness strategy
            if (ExtensionSupported("GL_ARB_robustness"))
            {
                //NOTE: we avoid using the context flags for detection, as they are
                //only present from 3.0 while the extension applies from 1.1

                GLint strategy;
                context->GetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_ARB, &strategy);

                if (strategy == GL_LOSE_CONTEXT_ON_RESET_ARB)
                {
                    context->m_Robustness = ContextRobustnessMode::LoseContextOnReset;
                }
                else if (strategy == GL_NO_RESET_NOTIFICATION_ARB)
                {
                    context->m_Robustness = ContextRobustnessMode::NoResetNotification;
                }
            }
        }
        else
        {
            //read back robustness strategy
            if (ExtensionSupported("GL_EXT_robustness"))
            {
                //NOTE: the values of these constants match those of the OpenGL ARB one,
                //so we can reuse them here

                GLint strategy;
                context->GetIntegerv(GL_RESET_NOTIFICATION_STRATEGY_ARB, &strategy);

                if (strategy == GL_LOSE_CONTEXT_ON_RESET_ARB)
                {
                    context->m_Robustness = ContextRobustnessMode::LoseContextOnReset;
                }
                else if (strategy == GL_NO_RESET_NOTIFICATION_ARB)
                {
                    context->m_Robustness = ContextRobustnessMode::NoResetNotification;
                }
            }
        }

        if (ExtensionSupported("GL_KHR_context_flush_control"))
        {
            GLint behavior;
            context->GetIntegerv(GL_CONTEXT_RELEASE_BEHAVIOR, &behavior);

            if (behavior == GL_NONE)
            {
                context->m_Release = ContextReleaseBehavior::None;
            }
            else if (behavior == GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH)
            {
                context->m_Release = ContextReleaseBehavior::Flush;
            }
        }

        //clearing the front buffer to black to avoid garbage pixels left over from
        //previous uses of our bit of VRAM
        {
            PFNGLCLEARPROC glClear = (PFNGLCLEARPROC)PlatformGetGLProcAddress("glClear");
            glClear(GL_COLOR_BUFFER_BIT);
            PlatformSwapBuffers(window);
        }

        MakeContextCurrent(previous);
        return true;
    }


    void Context::MakeContextCurrent(Window* window)
    {
        Window* previous = GetCurrentContext();

        if (window
            && window->m_Context->m_API == ContextAPI::None)
        {
            CPP_GLFW_ERROR("Cannot make current with a window that has no OpenGL or OpenGL ES context!");
            return;
        }

        if (previous)
        {
            if (!window
                || window->m_Context->m_Type != previous->m_Context->m_Type)
            {
                if (previous->m_Context->m_Type == ContextType::Native)
                {
                    PlatformMakeContextCurrent(nullptr);
                }
                else if (previous->m_Context->m_Type == ContextType::EGL)
                {
                    EglContext::EGLMakeContextCurrent(nullptr);
                }
            }
        }

        if (window)
        {
            if (window->m_Context->m_Type == ContextType::Native)
            {
                PlatformMakeContextCurrent(window);
            }
            else if (window->m_Context->m_Type == ContextType::EGL)
            {
                EglContext::EGLMakeContextCurrent(window);
            }
        }
    }

    void Context::SwapBuffers(Window* window)
    {
        if (window->m_Context->m_API == ContextAPI::None)
        {
            CPP_GLFW_ERROR("Cannot swap buffers of a window that has no OpenGL or OpenGL ES context!");
            return;
        }

        if (window->m_Context->m_Type == ContextType::Native)
        {
            PlatformSwapBuffers(window);
        }
        else if (window->m_Context->m_Type == ContextType::EGL)
        {
            EglContext::EGLSwapBuffers(window);
        }
    }

    void Context::SwapInterval(int32_t interval)
    {
        Window* window = GetCurrentContext();
        if (!window)
        {
            CPP_GLFW_ERROR("Cannot set swap interval without a current OpenGL or OpenGL ES context!");
            return;
        }

        if (window->m_Context->m_Type == ContextType::Native)
        {
            PlatformSwapInterval(interval);
        }
        else if (window->m_Context->m_Type == ContextType::EGL)
        {
            EglContext::EGLSwapInterval(interval);
        }
    }

    bool Context::ExtensionSupported(const char* extension)
    {
        Window* window = GetCurrentContext();
        if (!window)
        {
            CPP_GLFW_ERROR("Cannot query extension without a current OpenGL or OpenGL ES context!");
            return false;
        }

        if (window->m_Context->m_Type == ContextType::Native)
        {
            if (*extension == '\0')
            {
                CPP_GLFW_ERROR("Extension name cannot be empty!");
                return false;
            }

            if (window->m_Context->m_Major >= 3)
            {
                //check if extension is in the modern OpenGL extensions string list
                GLint count;
                window->m_Context->GetIntegerv(GL_NUM_EXTENSIONS, &count);

                for (int32_t i = 0; i < count; i++)
                {
                    const char* en = (const char*)window->m_Context->GetStringi(GL_EXTENSIONS, i);
                    if (!en)
                    {
                        CPP_GLFW_ERROR("Extension string retrieval is broken!");
                        return false;
                    }

                    if (strcmp(en, extension) == 0)
                    {
                        return true;
                    }
                }
            }
            else
            {
                //check if extension is in the old style OpenGL extension string
                const char* extensions = (const char*)window->m_Context->GetString(GL_EXTENSIONS);
                if (!extensions)
                {
                    CPP_GLFW_ERROR("Extension string retrieval is broken!");
                    return false;
                }

                if (StringInExtensionString(extension, extensions))
                {
                    return true;
                }
            }

            //check if extension is in the platform-specific string
            return PlatformExtensionSupported(extension);
        }
        else if (window->m_Context->m_Type == ContextType::EGL)
        {
            return EglContext::EGLExtensionSupported(extension);
        }
    }

    GLProc Context::GetGLProcAddress(const char* procedureName)
    {
        Window* window = GetCurrentContext();
        if (!window)
        {
            CPP_GLFW_ERROR("Cannot query entry point without a current OpenGL or OpenGL ES context!");
            return nullptr;
        }

        if (window->m_Context->m_Type == ContextType::Native)
        {
            return PlatformGetGLProcAddress(procedureName);
        }
        else if (window->m_Context->m_Type == ContextType::EGL)
        {
            return EglContext::EGLGetGLProcAddress(procedureName);
        }
    }

    void Context::DestroyContext(Window* window)
    {
        if (window->m_Context->m_API == ContextAPI::None)
        {
            CPP_GLFW_ERROR("Cannot destroy context of a window that has no OpenGL or OpenGL ES context!");
            return;
        }

        if (window->m_Context->m_Type == ContextType::Native)
        {
            PlatformDestroyContext(window);
        }
        else if (window->m_Context->m_Type == ContextType::EGL)
        {
            EglContext::DestroyContext(window);
        }
    }
}

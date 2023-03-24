#include "engine/core/Platform.h"

namespace cpp_glfw
{
    bool Input::s_JoysticksInitialized = false;

    bool Input::InitJoysticks()
    {
        if (!Input::s_JoysticksInitialized)
        {
            if (!Input::PlatformInitJoystycks())
            {
                Input::PlatformTerminateJoystycks();
                return false;
            }
        }

        Input::s_JoysticksInitialized = true;
        return true;
    }
}

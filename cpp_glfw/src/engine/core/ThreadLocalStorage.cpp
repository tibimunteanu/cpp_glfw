#include "Platform.h"

namespace cpp_glfw
{
    void* ThreadLocalStorage::Get()
    {
        return PlatformGet();
    }

    void ThreadLocalStorage::Set(void* value)
    {
        PlatformSet(value);
    }
}

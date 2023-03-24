#pragma once

#include "platform/windows/WindowsBase.h"

namespace cpp_glfw
{
    class WindowsThreadLocalStorage : public ThreadLocalStorage
    {
    public:
        bool m_Allocated;
        DWORD m_Index;

    public:
        WindowsThreadLocalStorage() = default;
        ~WindowsThreadLocalStorage();

    private:
        void* PlatformGet() override;
        void PlatformSet(void* value) override;
    };
}

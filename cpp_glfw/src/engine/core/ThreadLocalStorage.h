#pragma once

#include "engine/core/Base.h"

namespace cpp_glfw
{
    class ThreadLocalStorage
    {
    public:
        static ThreadLocalStorage* Create();

    public:
        ThreadLocalStorage() = default;
        virtual ~ThreadLocalStorage() = default;

    public:
        void* Get();
        void Set(void* value);

    protected:
        virtual void* PlatformGet() = 0;
        virtual void PlatformSet(void* value) = 0;
    };
}

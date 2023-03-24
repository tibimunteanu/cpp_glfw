#pragma once

#include "Base.h"

namespace cpp_glfw
{
    class Cursor
    {
    public:
        static Cursor* Create(const Image* image, int32_t xHot, int32_t yHot);
        static Cursor* Create(CursorShape shape);

    public:
        virtual ~Cursor() = default;
    };
}

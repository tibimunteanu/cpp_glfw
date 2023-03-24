#include "engine/core/Platform.h"

namespace cpp_glfw
{
    ////////////////////////////////////// STATIC INIT ///////////////////////////////////////////

    Platform::Hints Platform::s_Hints = {};
    std::vector<Window*> Platform::s_Windows = {};
    std::vector<Monitor*> Platform::s_Monitors = {};
    std::vector<Cursor*> Platform::s_Cursors = {};
    uint64_t Platform::s_TimerOffset = 0;
    ThreadLocalStorage* Platform::s_ContextSlot = nullptr;
    Platform::Callbacks Platform::s_Callbacks = {};



    //////////////////////////////////////// STATIC API ///////////////////////////////////////////

    bool Platform::Init()
    {
        if (!Platform::PlatformInit())
        {
            return false;
        }

        s_ContextSlot = ThreadLocalStorage::Create();
        if (!s_ContextSlot)
        {
            return false;
        }

        s_TimerOffset = PlatformGetTimerValue();

        SetHintsToDefult();

        return true;
    }

    void Platform::Terminate()
    {
        if (s_Monitors.size() > 0)
        {
            for (size_t i = 0; i < s_Monitors.size(); i++)
            {
                s_Monitors[i]->RestoreOriginalGammaRamp();
                delete s_Monitors[i];
            }
        }

        if (s_Windows.size() > 0)
        {
            for (size_t i = 0; i < s_Windows.size(); i++)
            {
                delete s_Windows[i];
            }
        }

        if (s_Cursors.size() > 0)
        {
            for (size_t i = 0; i < s_Cursors.size(); i++)
            {
                delete s_Cursors[i];
            }
        }

        delete s_ContextSlot;

        Platform::PlatformTerminate();

        EglContext::Terminate();
    }

    void Platform::PollEvents()
    {
        Platform::PlatformPollEvents();
    }

    void Platform::WaitEvents()
    {
        Platform::PlatformWaitEvents();
    }

    void Platform::WaitEventsTimeout(double timeout)
    {
        Platform::PlatformWaitEventsTimeout(timeout);
    }

    void Platform::SetHintsToDefult()
    {
        s_Hints = {};
        s_Hints.refreshRate = -1;
        s_Hints.window.resizable = true;
        s_Hints.window.visible = true;
        s_Hints.window.decorated = true;
        s_Hints.window.focused = true;
        s_Hints.window.autoMinimize = true;
        s_Hints.window.centerCursor = true;
        s_Hints.window.focusOnShow = true;
        s_Hints.window.scaleToMonitor = true;
        s_Hints.context.api = ContextAPI::OpenGL;
        s_Hints.context.type = ContextType::Native;
        s_Hints.context.major = 1;
        s_Hints.context.minor = 0;
        s_Hints.framebuffer.redBits = 8;
        s_Hints.framebuffer.greenBits = 8;
        s_Hints.framebuffer.blueBits = 8;
        s_Hints.framebuffer.alphaBits = 8;
        s_Hints.framebuffer.depthBits = 24;
        s_Hints.framebuffer.stencilBits = 8;
        s_Hints.framebuffer.doubleBuffer = true;
        s_Hints.framebuffer.sRGB = true;
    }


    bool Platform::IsRawMouseMotionSupported()
    {
        return Platform::PlatformIsRawMouseMotionSupported();
    }


    Window* Platform::OpenWindow(const std::string& title, int32_t width, int32_t height, Monitor* monitor)
    {
        return OpenWindow(title, width, height, &s_Hints.window, &s_Hints.context, &s_Hints.framebuffer, monitor);
    }

    Window* Platform::OpenWindow(const std::string& title, int32_t width, int32_t height,
        const WindowConfig* windowConfig, const ContextConfig* contextConfig, const FramebufferConfig* framebufferConfig, Monitor* monitor)
    {
        Window* window = Window::Create(title, width, height, windowConfig, contextConfig, framebufferConfig, monitor);
        if (window)
        {
            s_Windows.push_back(window);
        }
        return window;
    }


    const std::vector<Window*>& Platform::GetWindows()
    {
        return s_Windows;
    }

    Window* Platform::GetPrimaryWindow()
    {
        return s_Windows.size() > 0
            ? s_Windows[0]
            : nullptr;
    }


    double Platform::GetTime()
    {
        return (double)(PlatformGetTimerValue() - s_TimerOffset) / PlatformGetTimerFrequency();
    }

    void Platform::SetTime(double time)
    {
        if (time != time
            || time < 0.0
            || time > 18446744073.0)
        {
            CPP_GLFW_ERROR("Invalid time %f", time);
            return;
        }

        s_TimerOffset = PlatformGetTimerValue() - (uint64_t)(time * PlatformGetTimerFrequency());
    }

    uint64_t Platform::GetTimerValue()
    {
        return PlatformGetTimerValue();
    }

    uint64_t Platform::GetTimerFrequency()
    {
        return PlatformGetTimerFrequency();
    }


    const std::vector<Monitor*>& Platform::GetMonitors()
    {
        return s_Monitors;
    }

    Monitor* Platform::GetPrimaryMonitor()
    {
        return s_Monitors.size() > 0
            ? s_Monitors[0]
            : nullptr;
    }


    const char* Platform::GetKeyName(Key key, int32_t scancode)
    {
        if (key != Key::Unknown)
        {
            if (key != Key::KeyPadEqual
                && (key < Key::KeyPad0 || key > Key::KeyPadAdd)
                && (key < Key::Apostrophe || key > Key::World2))
            {
                return nullptr;
            }

            scancode = PlatformGetKeyScancode(key);
        }

        return PlatformGetScancodeName(scancode);
    }

    int32_t Platform::GetKeyScancode(Key key)
    {
        return PlatformGetKeyScancode(key);
    }


    const std::vector<std::string>& Platform::GetEglLibNames()
    {
        return PlatformGetEglLibNames();
    }

    const std::vector<std::string>& Platform::GetGLES1LibNames()
    {
        return PlatformGetGLES1LibNames();
    }

    const std::vector<std::string>& Platform::GetGLES2LibNames()
    {
        return PlatformGetGLES2LibNames();
    }

    const std::vector<std::string>& Platform::GetGLSLibNames()
    {
        return PlatformGetGLSLibNames();
    }


    EGLenum Platform::GetEglPlatform(EGLint** attribs)
    {
        return PlatformGetEglPlatform(attribs);
    }

    EGLNativeDisplayType Platform::GetEglNativeDisplay()
    {
        return PlatformGetEglNativeDisplay();
    }

    EGLNativeWindowType Platform::GetEglNativeWindow(Window* window)
    {
        return PlatformGetEglNativeWindow(window);
    }


    Cursor* Platform::CreateCursor(const Image* image, int32_t xHot, int32_t yHot)
    {
        Cursor* cursor = Cursor::Create(image, xHot, yHot);
        if (cursor)
        {
            s_Cursors.push_back(cursor);
        }
        return cursor;
    }

    Cursor* Platform::CreateStandardCursor(CursorShape shape)
    {
        Cursor* cursor = Cursor::Create(shape);
        if (cursor)
        {
            s_Cursors.push_back(cursor);
        }
        return cursor;
    }

    void Platform::DestroyCursor(Cursor* cursor)
    {
        //make sure the cursor is not being used by any window
        for (Window* window : s_Windows)
        {
            if (window->m_Cursor == cursor)
            {
                window->SetCursorType(nullptr);
            }
        }

        for (uint32_t i = 0; i < s_Cursors.size(); i++)
        {
            if (s_Cursors[i] == cursor)
            {
                s_Cursors.erase(s_Cursors.begin() + i);
                break;
            }
        }

        delete cursor;
    }


    const char* Platform::GetClipboardString()
    {
        return PlatformGetClipboardString();
    }

    void Platform::SetClipboardString(const char* string)
    {
        PlatformSetClipboardString(string);
    }


    void Platform::SetMonitorConnectedCallback(MonitorCallback callback)
    {
        s_Callbacks.monitorConnected = callback;
    }

    void Platform::SetMonitorDisconnectedCallback(MonitorCallback callback)
    {
        s_Callbacks.monitorDisconnected = callback;
    }
}


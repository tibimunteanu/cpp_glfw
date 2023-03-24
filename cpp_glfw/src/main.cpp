#include "engine/core/Platform.h"

#include "external/glad.h"

static void OnMonitorConnected(cpp_glfw::Monitor *monitor)
{
    std::cout << "[MAIN] OnMonitorConnected: " << monitor->GetName() << std::endl;
}
static void OnMonitorDisconnected(cpp_glfw::Monitor *monitor)
{
    std::cout << "[MAIN] OnMonitorDisconnected: " << monitor->GetName() << std::endl;
}
static void OnWindowPositionChanged(cpp_glfw::Window *window, int32_t x, int32_t y)
{
    std::cout << "[MAIN] OnWindowPosition: " << x << ", " << y << std::endl;
}
static void OnWindowSizeChanged(cpp_glfw::Window *window, int32_t width, int32_t height)
{
    std::cout << "[MAIN] OnWindowSize: " << width << ", " << height << std::endl;
}
static void OnWindowClose(cpp_glfw::Window *window)
{
    std::cout << "[MAIN] OnWindowClose" << std::endl;
}
static void OnWindowRefreshNeeded(cpp_glfw::Window *window)
{
    std::cout << "[MAIN] OnWindowRefresh" << std::endl;
}
static void OnWindowFocus(cpp_glfw::Window *window, bool focused)
{
    std::cout << "[MAIN] OnWindowFocus: " << focused << std::endl;
}
static void OnWindowMinimize(cpp_glfw::Window *window, bool minimized)
{
    std::cout << "[MAIN] OnWindowMinimize: " << minimized << std::endl;
}
static void OnWindowMaximize(cpp_glfw::Window *window, bool maximized)
{
    std::cout << "[MAIN] OnWindowMaximize: " << maximized << std::endl;
}
static void OnWindowFramebufferSizeChanged(cpp_glfw::Window *window, int32_t width, int32_t height)
{
    std::cout << "[MAIN] OnWindowFramebufferSize: " << width << ", " << height << std::endl;
}
static void OnWindowContentScaleChanged(cpp_glfw::Window *window, float xScale, float yScale)
{
    std::cout << "[MAIN] OnWindowContentScale: " << xScale << ", " << yScale << std::endl;
}
static void OnWindowMouseButton(cpp_glfw::Window *window, cpp_glfw::MouseButton button, cpp_glfw::KeyState state, cpp_glfw::KeyMods mods)
{
    std::cout
        << "[MAIN] OnWindowMouseButton: "
        << (int32_t)button
        << ", "
        << (state == cpp_glfw::KeyState::Press ? "Press" : state == cpp_glfw::KeyState::Release ? "Release"
                                                                                                : "Repeat")
        << ((mods & cpp_glfw::KeyMods::Alt) == cpp_glfw::KeyMods::Alt ? ":Alt:" : "")
        << ((mods & cpp_glfw::KeyMods::Shift) == cpp_glfw::KeyMods::Shift ? ":Shift:" : "")
        << ((mods & cpp_glfw::KeyMods::Control) == cpp_glfw::KeyMods::Control ? ":Ctrl:" : "")
        << ((mods & cpp_glfw::KeyMods::Super) == cpp_glfw::KeyMods::Super ? ":Super:" : "")
        << ((mods & cpp_glfw::KeyMods::CapsLock) == cpp_glfw::KeyMods::CapsLock ? ":CapsLock:" : "")
        << ((mods & cpp_glfw::KeyMods::NumLock) == cpp_glfw::KeyMods::NumLock ? ":NumLock:" : "")
        << std::endl;
}
static void OnCursorPositionChanged(cpp_glfw::Window *window, double x, double y)
{
    std::cout << "[MAIN] OnCursorPositionChanged: " << x << ", " << y << std::endl;
}
static void OnWindowCursorEnter(cpp_glfw::Window *window, bool entered)
{
    std::cout << "[MAIN] OnWindowCursorEnter: " << entered << std::endl;
}
static void OnWindowScroll(cpp_glfw::Window *window, double xOffset, double yOffset)
{
    std::cout << "[MAIN] OnWindowScroll: " << xOffset << ", " << yOffset << std::endl;
}
static void OnWindowKey(cpp_glfw::Window *window, cpp_glfw::Key key, int32_t scancode, cpp_glfw::KeyState state, cpp_glfw::KeyMods mods)
{
    std::cout
        << "[MAIN] OnWindowKey: "
        << (int32_t)key
        << ", "
        << (state == cpp_glfw::KeyState::Press ? "Press" : state == cpp_glfw::KeyState::Release ? "Release"
                                                                                                : "Repeat")
        << ((mods & cpp_glfw::KeyMods::Alt) == cpp_glfw::KeyMods::Alt ? ":Alt:" : "")
        << ((mods & cpp_glfw::KeyMods::Shift) == cpp_glfw::KeyMods::Shift ? ":Shift:" : "")
        << ((mods & cpp_glfw::KeyMods::Control) == cpp_glfw::KeyMods::Control ? ":Ctrl:" : "")
        << ((mods & cpp_glfw::KeyMods::Super) == cpp_glfw::KeyMods::Super ? ":Super:" : "")
        << ((mods & cpp_glfw::KeyMods::CapsLock) == cpp_glfw::KeyMods::CapsLock ? ":CapsLock:" : "")
        << ((mods & cpp_glfw::KeyMods::NumLock) == cpp_glfw::KeyMods::NumLock ? ":NumLock:" : "")
        << std::endl;
}
static void OnWindowChar(cpp_glfw::Window *window, uint32_t codepoint)
{
    std::cout << "[MAIN] OnWindowChar: " << (char)codepoint << std::endl;
}

int main(int argc, char **argv)
{
    cpp_glfw::Platform::SetMonitorConnectedCallback(OnMonitorConnected);
    cpp_glfw::Platform::SetMonitorDisconnectedCallback(OnMonitorDisconnected);

    cpp_glfw::Platform::s_Hints = {};
    cpp_glfw::Platform::s_Hints.init.angleType = cpp_glfw::AnglePlatformType::None;

    cpp_glfw::Platform::Init();

    // test monitor api
    std::cout << "Connected monitors: " << std::endl;
    const std::vector<cpp_glfw::Monitor *> &monitors = cpp_glfw::Platform::GetMonitors();
    for (int32_t m = 0; m < monitors.size(); m++)
    {
        std::cout << "\t" << monitors[m]->GetName() << std::endl;
    }

    cpp_glfw::Monitor *currentMonitor = cpp_glfw::Platform::GetPrimaryMonitor();
    if (currentMonitor)
    {
        std::cout << "Current monitor: " << currentMonitor->GetName() << std::endl;

        const std::vector<cpp_glfw::VideoMode *> &videoModes = currentMonitor->GetVideoModes();

        std::cout << "Video modes (" << videoModes.size() << "):" << std::endl;
        for (int32_t m = 0; m < videoModes.size(); m++)
        {
            std::cout << "\t" << *videoModes[m] << std::endl;
        }

        cpp_glfw::VideoMode *currentVideoMode = currentMonitor->GetVideoMode();
        if (currentVideoMode)
        {
            std::cout << "Current video mode: " << *currentVideoMode << std::endl;
        }

        int32_t x, y;
        currentMonitor->GetPosition(&x, &y);
        std::cout << "Position: x: " << x << ", y: " << y << std::endl;

        int32_t w, h;
        currentMonitor->GetWorkarea(&x, &y, &w, &h);
        std::cout << "Workarea: x: " << x << ", y: " << y << ", w: " << w << ", h: " << h << std::endl;

        currentMonitor->GetPhysicalSize(&w, &h);
        std::cout << "Physical size: w: " << w << "mm, h: " << h << "mm" << std::endl;

        float xScale, yScale;
        currentMonitor->GetContentScale(&xScale, &yScale);
        std::cout << "Content scale: xScale: " << xScale << ", yScale: " << yScale << std::endl;
    }

    // test window api
    cpp_glfw::Platform::s_Hints.refreshRate = -1;

    cpp_glfw::Platform::s_Hints.window.resizable = true;
    cpp_glfw::Platform::s_Hints.window.visible = true;
    cpp_glfw::Platform::s_Hints.window.decorated = true;
    cpp_glfw::Platform::s_Hints.window.focused = true;
    cpp_glfw::Platform::s_Hints.window.autoMinimize = true;
    cpp_glfw::Platform::s_Hints.window.centerCursor = true;
    cpp_glfw::Platform::s_Hints.window.focusOnShow = true;
    cpp_glfw::Platform::s_Hints.window.scaleToMonitor = true;

    cpp_glfw::Platform::s_Hints.context.api = cpp_glfw::ContextAPI::OpenGL;
    cpp_glfw::Platform::s_Hints.context.type = cpp_glfw::ContextType::Native;
    cpp_glfw::Platform::s_Hints.context.profile = cpp_glfw::ContextProfile::Core;
    cpp_glfw::Platform::s_Hints.context.robustness = cpp_glfw::ContextRobustnessMode::None;
    cpp_glfw::Platform::s_Hints.context.release = cpp_glfw::ContextReleaseBehavior::Any;
    cpp_glfw::Platform::s_Hints.context.debug = false;
    cpp_glfw::Platform::s_Hints.context.noerror = false;
    cpp_glfw::Platform::s_Hints.context.forward = false;
    cpp_glfw::Platform::s_Hints.context.major = 1;
    cpp_glfw::Platform::s_Hints.context.minor = 0;

    cpp_glfw::Platform::s_Hints.framebuffer.redBits = 8;
    cpp_glfw::Platform::s_Hints.framebuffer.greenBits = 8;
    cpp_glfw::Platform::s_Hints.framebuffer.blueBits = 8;
    cpp_glfw::Platform::s_Hints.framebuffer.alphaBits = 8;
    cpp_glfw::Platform::s_Hints.framebuffer.depthBits = 24;
    cpp_glfw::Platform::s_Hints.framebuffer.stencilBits = 8;
    cpp_glfw::Platform::s_Hints.framebuffer.doubleBuffer = true;
    cpp_glfw::Platform::s_Hints.framebuffer.sRGB = true;

    cpp_glfw::Window *window = cpp_glfw::Platform::OpenWindow("Cpp_GLFW", 960, 540, nullptr);
    if (!window)
    {
        std::cout << "Could not open window!" << std::endl;
        return 1;
    }

    bool hasContext = cpp_glfw::Platform::s_Hints.context.api != cpp_glfw::ContextAPI::None;
    if (hasContext)
    {
        std::cout
            << "Current context: OpenGL "
            << window->GetContext()->m_Major << "." << window->GetContext()->m_Minor << "." << window->GetContext()->m_Revision
            << std::endl;

        cpp_glfw::Context::MakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)cpp_glfw::Context::GetGLProcAddress))
        {
            std::cout << "Could not initialize OpenGL loader!" << std::endl;
            return 1;
        }

        cpp_glfw::Context::SwapInterval(1);
    }

    window->SetCharCallback(OnWindowChar);
    window->SetKeyCallback(OnWindowKey);
    // window->SetCursorPositionCallback(OnCursorPositionChanged);
    window->SetCloseCallback(OnWindowClose);
    window->SetContentScaleCallback(OnWindowContentScaleChanged);
    window->SetCursorEnterCallback(OnWindowCursorEnter);
    window->SetFocusCallback(OnWindowFocus);
    window->SetFramebufferSizeCallback(OnWindowFramebufferSizeChanged);
    window->SetMaximizeCallback(OnWindowMaximize);
    window->SetMinimizeCallback(OnWindowMinimize);
    window->SetMouseButtonCallback(OnWindowMouseButton);
    window->SetPositionCallback(OnWindowPositionChanged);
    window->SetScrollCallback(OnWindowScroll);
    window->SetSizeCallback(OnWindowSizeChanged);

    // loop
    double prevTime = cpp_glfw::Platform::GetTime() - (1.0 / 60.0);
    double dt;
    while (!window->ShouldClose())
    {
        double time = cpp_glfw::Platform::GetTime();
        dt = time - prevTime;
        prevTime = time;

        if (hasContext)
        {
            glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            cpp_glfw::Context::SwapBuffers(window);
        }

        cpp_glfw::Platform::PollEvents();
    }

    cpp_glfw::Platform::Terminate();

    return 0;
}

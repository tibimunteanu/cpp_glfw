#pragma once

#include "engine/core/Platform.h"
#include "platform/windows/WindowsBase.h"
#include "platform/windows/WindowsThreadLocalStorage.h"
#include "platform/windows/WindowsWglContext.h"
#include "platform/windows/WindowsCursor.h"
#include "platform/windows/WindowsMonitor.h"
#include "platform/windows/WindowsWindow.h"

namespace cpp_glfw
{
    class WindowsPlatform : public Platform
    {
    public:
        static HWND s_HelperWindowHandle;
        static HDEVNOTIFY s_DeviceNotificationHandle;
        static DWORD s_ForegroundLockTimeout;
        static int32_t s_AcquiredMonitorCount;
        static bool s_TimerHasPC;
        static uint64_t s_TimerFrequency;
        static char* s_ClipboardString;
        static Key s_Keycodes[512];
        static int16_t s_Scancodes[(int32_t)Key::Count];
        static char s_KeyNames[(int32_t)Key::Count][5];
        static double s_RestoreCursorPositionX; //where to place the cursor when re-enabled
        static double s_RestoreCursorPositionY;
        static Window* s_DisabledCursorWindow; //the window whose disabled cursor mode is active
        static RAWINPUT* s_RawInput;
        static int32_t s_RawInputSize;
        static UINT s_MouseTrailSize;
        static std::vector<std::string> s_EglLibNames;
        static std::vector<std::string> s_GLES1LibNames;
        static std::vector<std::string> s_GLES2LibNames;
        static std::vector<std::string> s_GLSLibNames;

        static struct WindowsLibs
        {
            struct XInputLib
            {
                HINSTANCE instance;
                PFN_XInputGetCapabilities GetCapabilities;
                PFN_XInputGetState GetState;
            } xInput;

            struct DInput8Lib
            {
                HINSTANCE instance;
                PFN_DirectInput8Create Create;
                IDirectInput8W* API;
            } dInput8;

            struct WinmmLib
            {
                HINSTANCE instance;
                PFN_timeGetTime GetTime;
            } winmm;

            struct User32Lib
            {
                HINSTANCE instance;
                PFN_SetProcessDPIAware SetProcessDPIAware;
                PFN_ChangeWindowMessageFilterEx ChangeWindowMessageFilterEx;
                PFN_EnableNonClientDpiScaling EnableNonClientDpiScaling;
                PFN_SetProcessDpiAwarenessContext SetProcessDpiAwarenessContext;
                PFN_GetDpiForWindow GetDpiForWindow;
                PFN_AdjustWindowRectExForDpi AdjustWindowRectExForDpi;
            } user32;

            struct DwmapiLib
            {
                HINSTANCE instance;
                PFN_DwmIsCompositionEnabled IsCompositionEnabled;
                PFN_DwmFlush Flush;
                PFN_DwmEnableBlurBehindWindow EnableBlurBehindWindow;
                PFN_DwmGetColorizationColor GetColorizationColor;
            } dwmapi;

            struct ShcoreLib
            {
                HINSTANCE instance;
                PFN_SetProcessDpiAwareness SetProcessDpiAwareness;
                PFN_GetDpiForMonitor GetDpiForMonitor;
            } shcore;

            struct NtdllLib
            {
                HINSTANCE instance;
                PFN_RtlVerifyVersionInfo RtlVerifyVersionInfo;
            } ntdll;
        } s_Libs;

    public: CPP_GLFW_INTERNAL_API
        static bool LoadLibraries();
        static void FreeLibraries();

        static void CreateKeyTables();
        static void UpdateKeyNames();

        static void SetForegroundLockTimeout();
        static void RestoreForegroundLockTimeout();
        static void SetProcessDpiAware();

        static void AdjustRect(RECT* rect, HWND windowHandle, DWORD style, DWORD styleEx);
        static void AdjustRect(RECT* rect, DWORD style, DWORD styleEx, UINT dpi);

        static void PollMonitors();

        static void InitTimer();

        static bool RegisterWindowClass();
        static void UnregisterWindowClass();
        static bool CreateHelperWindow();

        static bool IsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp);
        static bool IsWindowsVistaOrGreater();
        static bool IsWindows7OrGreater();
        static bool IsWindows8OrGreater();
        static bool IsWindows8Point1OrGreater();
        static bool IsWindows10BuildOrGreater(WORD build);
        static bool IsWindows10AnniversaryUpdateOrGreater();
        static bool IsWindows10CreatorsUpdateOrGreater();

        static char* WideStringToUTF8(const WCHAR* source);
        static bool WideStringToUTF8(const WCHAR* source, std::string& target);
        static bool WideStringToUTF8(const WCHAR source[], char target[]);
        static bool WideStringToUTF8(const WCHAR source[], char target[], int32_t processCharCount);
        static WCHAR* UTF8ToWideString(const char* source);
    };
}


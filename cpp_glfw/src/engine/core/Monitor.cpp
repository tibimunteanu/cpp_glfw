#include "engine/core/Platform.h"

namespace cpp_glfw
{
    //////////////////////////////////////// STATIC ///////////////////////////////////////////

    void Monitor::SplitBPP(int32_t bpp, int32_t* red, int32_t* green, int32_t* blue)
    {
        int32_t delta;

        //we assume that by 32 the user really meant 24
        if (bpp == 32) bpp = 24;

        //convert "bits per pixel" to red, gree and blue sizes
        *red = *green = *blue = bpp / 3;
        delta = bpp - (*red * 3);
        if (delta >= 1)
        {
            *green = *green + 1;
        }
        if (delta == 2)
        {
            *red = *red + 1;
        }
    }



    ////////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////

    Monitor::Monitor()
    {
    }

    Monitor::~Monitor()
    {
        if (m_VideoModes.size())
        {
            for (int32_t i = 0; i < m_VideoModes.size(); i++)
            {
                delete m_VideoModes[i];
            }
            m_VideoModes.clear();
        }
    }



    /////////////////////////////////////// PUBLIC API ////////////////////////////////////////

    const std::string& Monitor::GetName() const
    {
        return m_Name;
    }


    void Monitor::GetPosition(int32_t* x, int32_t* y) const
    {
        return PlatformGetPosition(x, y);
    }

    void Monitor::GetWorkarea(int32_t* x, int32_t* y, int32_t* width, int32_t* height) const
    {
        return PlatformGetWorkarea(x, y, width, height);
    }

    void Monitor::GetContentScale(float* xScale, float* yScale) const
    {
        return PlatformGetContentScale(xScale, yScale);
    }

    void Monitor::GetPhysicalSize(int32_t* widthInMillimeters, int32_t* heightInMillimeters) const
    {
        *widthInMillimeters = m_WidthInMillimeters;
        *heightInMillimeters = m_HeightInMillimeters;
    }


    const std::vector<VideoMode*>& Monitor::GetVideoModes()
    {
        RefreshVideoModes();

        return m_VideoModes;
    }

    VideoMode* Monitor::GetVideoMode()
    {
        PlatformGetVideoMode(&m_CurrentVideoMode);

        return &m_CurrentVideoMode;
    }

    void Monitor::SetVideoMode(const VideoMode* videoMode)
    {
        PlatformSetVideoMode(videoMode);
    }

    void Monitor::RestoreVideoMode()
    {
        PlatformRestoreVideoMode();
    }


    void Monitor::SetGamma(float gamma)
    {
        if (gamma != gamma
            || gamma <= 0.0f
            || gamma > FLT_MAX)
        {
            CPP_GLFW_ERROR("Invalid gamma value %f!", gamma);
            return;
        }

        const GammaRamp* currentRamp = GetGammaRamp();

        if (!currentRamp)
        {
            return;
        }

        GammaRamp* ramp = new GammaRamp(currentRamp->size);

        for (uint32_t i = 0; i < currentRamp->size; i++)
        {
            //calculate intensity
            float value = i / (float)(currentRamp->size - 1);

            //apply gamma curve
            value = powf(value, 1.0f / gamma) * 65535.0f + 0.5f;

            //clamp to value range
            value = Utils::fminf(value, 65535.0f);

            ramp->red.push_back((uint16_t)value);
            ramp->green.push_back((uint16_t)value);
            ramp->blue.push_back((uint16_t)value);
        }

        SetGammaRamp(ramp);

        delete ramp;
    }

    const GammaRamp* Monitor::GetGammaRamp()
    {
        m_CurrentGammaRamp.Clear();

        PlatformGetGammaRamp(&m_CurrentGammaRamp);

        return &m_CurrentGammaRamp;
    }

    void Monitor::SetGammaRamp(GammaRamp* ramp)
    {
        if (!ramp
            || !ramp->IsValid())
        {
            CPP_GLFW_ERROR("Invalid gamma ramp!");
            return;
        }

        //make sure we have the original ramp
        if (!m_OriginalGammaRamp.size
            && !PlatformGetGammaRamp(&m_OriginalGammaRamp))
        {
            return;
        }

        PlatformSetGammaRamp(ramp);
    }

    void Monitor::RestoreOriginalGammaRamp()
    {
        if (m_OriginalGammaRamp.size)
        {
            PlatformSetGammaRamp(&m_OriginalGammaRamp);
        }
    }


    Window* Monitor::GetWindow() const
    {
        return m_Window;
    }

    void Monitor::SetWindow(Window* window)
    {
        m_Window = window;
    }



    ///////////////////////////////////////// UTILS ///////////////////////////////////////////

    bool Monitor::RefreshVideoModes()
    {
        if (m_VideoModes.size())
        {
            //since video modes will not change at runtime, this method's purpose is only for lazy loading
            return true;
        }

        //clear existing video modes
        if (m_VideoModes.size())
        {
            for (int32_t i = 0; i < m_VideoModes.size(); i++)
            {
                delete m_VideoModes[i];
            }
            m_VideoModes.clear();
        }

        PlatformGetVideoModes(m_VideoModes);

        if (!m_VideoModes.size())
        {
            return false;
        }

        std::sort(m_VideoModes.begin(), m_VideoModes.end(), [](VideoMode* a, VideoMode* b) { return *a < *b; });
        return true;
    }

    const VideoMode* Monitor::GetClosestVideoMode(const VideoMode* videoMode)
    {
        if (!RefreshVideoModes())
        {
            return nullptr;
        }

        uint32_t leastColorDiff = UINT_MAX;
        uint32_t leastSizeDiff = UINT_MAX;
        uint32_t leastRefreshRateDiff = UINT_MAX;
        VideoMode* closestVideoMode = nullptr;

        for (int i = 0; i < m_VideoModes.size(); i++)
        {
            VideoMode* currentVideoMode = m_VideoModes[i];

            uint32_t colorDiff = 0;
            if (videoMode->redBits != -1)
            {
                colorDiff += abs(currentVideoMode->redBits - videoMode->redBits);
            }
            if (videoMode->greenBits != -1)
            {
                colorDiff += abs(currentVideoMode->greenBits - videoMode->greenBits);
            }
            if (videoMode->blueBits != -1)
            {
                colorDiff += abs(currentVideoMode->blueBits - videoMode->blueBits);
            }

            int32_t widthDiff = currentVideoMode->width - videoMode->width;
            int32_t heightDiff = currentVideoMode->height - videoMode->height;
            uint32_t sizeDiff = abs(widthDiff * widthDiff + heightDiff * heightDiff);

            uint32_t refreshRateDiff = videoMode->refreshRate != -1
                ? abs(currentVideoMode->refreshRate - videoMode->refreshRate)
                : UINT_MAX - currentVideoMode->refreshRate;

            if ((colorDiff < leastColorDiff)
                || (colorDiff == leastColorDiff && sizeDiff < leastSizeDiff)
                || (colorDiff == leastColorDiff && sizeDiff == leastSizeDiff && refreshRateDiff < leastRefreshRateDiff))
            {
                closestVideoMode = currentVideoMode;
                leastColorDiff = colorDiff;
                leastSizeDiff = sizeDiff;
                leastRefreshRateDiff = refreshRateDiff;
            }
        }

        return closestVideoMode;
    }
}

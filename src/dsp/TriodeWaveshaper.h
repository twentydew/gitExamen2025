#pragma once

#include <cmath>

class TriodeWaveshaper
{
public:
    void setDrive(float dB) { drive = dbToLinear(dB); }
    void setAsymmetry(float a) { asymmetry = a; }

    inline float processSample(float x)
    {
        // Pre-emphasis and asymmetry to mimic triode-like behavior
        float biased = x + asymmetry * 0.1f;
        float driven = biased * drive;
        // Soft knee saturation
        float y = std::tanh(driven);
        // De-emphasis
        return y * 0.9f;
    }

private:
    float drive { 1.0f };
    float asymmetry { 0.0f };

    static inline float dbToLinear(float dB) { return std::pow(10.0f, dB / 20.0f); }
};
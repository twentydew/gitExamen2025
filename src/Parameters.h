#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace Params
{
    static constexpr const char* gain = "gain";
    static constexpr const char* bass = "bass";
    static constexpr const char* mid = "mid";
    static constexpr const char* treble = "treble";
    static constexpr const char* presence = "presence";
    static constexpr const char* volume = "volume";
    static constexpr const char* master = "master";
    static constexpr const char* bypass = "bypass";

    inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        using namespace juce;
        std::vector<std::unique_ptr<RangedAudioParameter>> params;

        params.push_back(std::make_unique<AudioParameterFloat>(
            ParameterID{ gain, 1 }, "Gain", NormalisableRange<float>{ 0.0f, 10.0f, 0.01f }, 5.0f));

        // Tone controls in dB
        params.push_back(std::make_unique<AudioParameterFloat>(
            ParameterID{ bass, 1 }, "Bass", NormalisableRange<float>{ -12.0f, 12.0f, 0.1f }, 0.0f));
        params.push_back(std::make_unique<AudioParameterFloat>(
            ParameterID{ mid, 1 }, "Mid", NormalisableRange<float>{ -12.0f, 12.0f, 0.1f }, 0.0f));
        params.push_back(std::make_unique<AudioParameterFloat>(
            ParameterID{ treble, 1 }, "Treble", NormalisableRange<float>{ -12.0f, 12.0f, 0.1f }, 0.0f));
        params.push_back(std::make_unique<AudioParameterFloat>(
            ParameterID{ presence, 1 }, "Presence", NormalisableRange<float>{ -12.0f, 12.0f, 0.1f }, 0.0f));

        // Output trims
        params.push_back(std::make_unique<AudioParameterFloat>(
            ParameterID{ volume, 1 }, "Volume", NormalisableRange<float>{ -24.0f, 24.0f, 0.1f }, 0.0f));
        params.push_back(std::make_unique<AudioParameterFloat>(
            ParameterID{ master, 1 }, "Master", NormalisableRange<float>{ -24.0f, 24.0f, 0.1f }, 0.0f));

        params.push_back(std::make_unique<AudioParameterBool>(
            ParameterID{ bypass, 1 }, "Bypass", false));

        return { params.begin(), params.end() };
    }
}
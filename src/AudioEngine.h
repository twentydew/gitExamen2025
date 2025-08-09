#pragma once

#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include "dsp/TriodeWaveshaper.h"
#include "dsp/ToneStack.h"

class AudioEngine
{
public:
    void prepare(double sampleRate, int samplesPerBlock, int numChannels)
    {
        juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), static_cast<juce::uint32> (numChannels) };
        toneStack.prepare(spec);
        oversampler = std::make_unique<juce::dsp::Oversampling<float>>(numChannels, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);
        oversampler->reset();
        dryWetMixer.prepare(spec);
        dryWetMixer.setMixingRule(juce::dsp::DryWetMixingRule::sin3dB);
        inputMeter.reset(sampleRate, 0.5);
        outputMeter.reset(sampleRate, 0.5);
        this->sampleRate = sampleRate;
    }

    void setParameters(float gain, float bass, float mid, float treble, float presence, float volume, float master, bool bypass)
    {
        triode.setDrive(juce::jmap(gain, 0.0f, 10.0f, -6.0f, 24.0f));
        triode.setAsymmetry(juce::jmap(gain, 0.0f, 10.0f, 0.05f, 0.25f));
        toneStack.setBass(bass);
        toneStack.setMid(mid);
        toneStack.setTreble(treble);
        toneStack.setPresence(presence);
        volumeTrim = juce::Decibels::decibelsToGain(volume);
        masterTrim = juce::Decibels::decibelsToGain(master);
        isBypassed = bypass;
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block (buffer);
        inputMeter.measureBlock(block);

        if (isBypassed)
        {
            // Apply only volume/master trims in bypass
            buffer.applyGain(volumeTrim * masterTrim);
            outputMeter.measureBlock(block);
            return;
        }

        auto oversampledBlock = oversampler->processSamplesUp(block);
        juce::dsp::AudioBlock<float> osBlock = oversampledBlock;

        // Non-linear processing per-sample for oversampled block
        for (size_t ch = 0; ch < osBlock.getNumChannels(); ++ch)
        {
            auto* data = osBlock.getChannelPointer(ch);
            for (size_t i = 0; i < osBlock.getNumSamples(); ++i)
            {
                data[i] = triode.processSample(data[i]);
            }
        }

        juce::dsp::ProcessContextReplacing<float> ctx (osBlock);
        toneStack.process(ctx);

        oversampler->processSamplesDown(block);

        buffer.applyGain(volumeTrim * masterTrim);
        outputMeter.measureBlock(block);
    }

    float getInputLevelDb() const { return inputMeter.getRMSLevelDb(); }
    float getOutputLevelDb() const { return outputMeter.getRMSLevelDb(); }

private:
    class RmsMeter
    {
    public:
        void reset(double sr, double smoothingSeconds)
        {
            sampleRate = sr;
            alpha = std::exp(-1.0 / (sr * smoothingSeconds));
            rms = 0.0;
        }

        void measureBlock(const juce::dsp::AudioBlock<float>& block)
        {
            const size_t n = block.getNumSamples();
            const size_t c = block.getNumChannels();
            double sumSquares = 0.0;
            for (size_t ch = 0; ch < c; ++ch)
            {
                auto* data = block.getChannelPointer(ch);
                for (size_t i = 0; i < n; ++i)
                    sumSquares += static_cast<double>(data[i]) * static_cast<double>(data[i]);
            }
            const double meanSquares = sumSquares / static_cast<double>(n * c);
            const double instRms = std::sqrt(meanSquares + 1e-12);
            rms = alpha * rms + (1.0 - alpha) * instRms;
        }

        float getRMSLevelDb() const { return juce::Decibels::gainToDecibels(static_cast<float>(rms) + 1e-9f); }
    private:
        double sampleRate { 44100.0 };
        double alpha { 0.99 };
        double rms { 0.0 };
    };

    TriodeWaveshaper triode;
    ToneStack toneStack;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    juce::dsp::DryWetMixer<float> dryWetMixer;

    RmsMeter inputMeter;
    RmsMeter outputMeter;

    double sampleRate { 44100.0 };
    float volumeTrim { 1.0f };
    float masterTrim { 1.0f };
    bool isBypassed { false };
};
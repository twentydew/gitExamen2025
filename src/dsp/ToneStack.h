#pragma once

#include <juce_dsp/juce_dsp.h>

class ToneStack
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        bassFilter.reset();
        midFilter.reset();
        trebleFilter.reset();
        presenceFilter.reset();
        bassFilter.prepare(spec);
        midFilter.prepare(spec);
        trebleFilter.prepare(spec);
        presenceFilter.prepare(spec);
        sampleRate = spec.sampleRate;
        updateFilters();
    }

    void setBass(float dB)     { bass = dB; updateFilters(); }
    void setMid(float dB)      { mid = dB; updateFilters(); }
    void setTreble(float dB)   { treble = dB; updateFilters(); }
    void setPresence(float dB) { presence = dB; updateFilters(); }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        bassFilter.process(context);
        midFilter.process(context);
        trebleFilter.process(context);
        presenceFilter.process(context);
    }

private:
    void updateFilters()
    {
        using Coeff = juce::dsp::IIR::Coefficients<float>;

        bassFilter.state = Coeff::makePeakFilter(sampleRate, 100.0f, 0.707f, juce::Decibels::decibelsToGain(bass));
        midFilter.state = Coeff::makePeakFilter(sampleRate, 800.0f, 1.0f, juce::Decibels::decibelsToGain(mid));
        trebleFilter.state = Coeff::makeHighShelf(sampleRate, 3000.0f, 0.707f, juce::Decibels::decibelsToGain(treble));
        presenceFilter.state = Coeff::makeHighShelf(sampleRate, 5000.0f, 0.707f, juce::Decibels::decibelsToGain(presence));
    }

    juce::dsp::IIR::Filter<float> bassFilter;
    juce::dsp::IIR::Filter<float> midFilter;
    juce::dsp::IIR::Filter<float> trebleFilter;
    juce::dsp::IIR::Filter<float> presenceFilter;

    double sampleRate { 44100.0 };
    float bass { 0.0f }, mid { 0.0f }, treble { 0.0f }, presence { 0.0f };
};
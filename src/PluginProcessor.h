#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "AudioEngine.h"
#include "Parameters.h"

class NeuralAmpAudioProcessor : public juce::AudioProcessor
{
public:
    NeuralAmpAudioProcessor();
    ~NeuralAmpAudioProcessor() override = default;

    // Basic AudioProcessor overrides
    const juce::String getName() const override { return "NeuralAmp"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts; }

    // Preset helpers
    void savePresetToFile(const juce::File& file);
    void loadPresetFromFile(const juce::File& file);

    float getInputMeterDb() const { return audioEngine.getInputLevelDb(); }
    float getOutputMeterDb() const { return audioEngine.getOutputLevelDb(); }

private:
    juce::AudioProcessorValueTreeState apvts;
    AudioEngine audioEngine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeuralAmpAudioProcessor)
};
#pragma once

#include <functional>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class MeterComponent : public juce::Component, private juce::Timer
{
public:
    MeterComponent(std::function<float()> inputDbFn, std::function<float()> outputDbFn)
    : inputDbProvider(std::move(inputDbFn)), outputDbProvider(std::move(outputDbFn))
    {
        startTimerHz(30);
    }

    void paint(juce::Graphics& g) override
    {
        auto r = getLocalBounds().reduced(2);
        auto w = r.getWidth() / 2;
        auto inR = r.removeFromLeft(w).reduced(2);
        auto outR = r.reduced(2);
        drawMeter(g, inR, inputDb);
        drawMeter(g, outR, outputDb);
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.drawFittedText("IN", inR.removeFromTop(16), juce::Justification::centred, 1);
        g.drawFittedText("OUT", outR.removeFromTop(16), juce::Justification::centred, 1);
    }

private:
    void timerCallback() override
    {
        inputDb = inputDbProvider();
        outputDb = outputDbProvider();
        repaint();
    }

    static void drawMeter(juce::Graphics& g, juce::Rectangle<int> area, float levelDb)
    {
        auto meterArea = area.withTrimmedTop(16);
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.fillRoundedRectangle(meterArea.toFloat(), 4.0f);

        float norm = juce::jlimit(0.0f, 1.0f, juce::jmap(levelDb, -60.0f, 0.0f, 0.0f, 1.0f));
        auto fill = meterArea.removeFromBottom(static_cast<int>(meterArea.getHeight() * norm));
        juce::Colour c = juce::Colours::limegreen;
        if (levelDb > -3.0f) c = juce::Colours::orangered;
        else if (levelDb > -12.0f) c = juce::Colours::goldenrod;
        g.setColour(c);
        g.fillRoundedRectangle(fill.toFloat(), 4.0f);
    }

    std::function<float()> inputDbProvider;
    std::function<float()> outputDbProvider;
    float inputDb { -60.0f };
    float outputDb { -60.0f };
};

class LookAndFeelModern : public juce::LookAndFeel_V4
{
public:
    LookAndFeelModern()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xffE9B872));
        setColour(juce::Slider::trackColourId, juce::Colour(0xff30475E));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffe94560));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff222831));
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xff30475E));
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    }
};

class LabeledKnob : public juce::Component
{
public:
    LabeledKnob(const juce::String& labelText, juce::RangedAudioParameter& param, juce::AudioProcessorValueTreeState& state)
    : slider(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow), label({}, labelText)
    {
        slider.setRange(param.getNormalisableRange().start, param.getNormalisableRange().end, param.getNormalisableRange().interval);
        slider.setSkewFactorFromMidPoint((param.getNormalisableRange().start + param.getNormalisableRange().end) * 0.5);
        slider.setTextValueSuffix(" " + param.getLabel());
        addAndMakeVisible(slider);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, param.getParameterID().getParamID(), slider);
    }

    void resized() override
    {
        auto r = getLocalBounds();
        label.setBounds(r.removeFromTop(20));
        slider.setBounds(r.reduced(4));
    }

    juce::Slider& getSlider() { return slider; }

private:
    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};

class NeuralAmpAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    explicit NeuralAmpAudioProcessorEditor (NeuralAmpAudioProcessor&);
    ~NeuralAmpAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override { meters.repaint(); }

    LookAndFeelModern lnf;

    LabeledKnob gainKnob, bassKnob, midKnob, trebleKnob, presenceKnob, volumeKnob, masterKnob;
    juce::ToggleButton bypassButton { "Bypass" };

    juce::TextButton saveButton { "Save Preset" };
    juce::TextButton loadButton { "Load Preset" };

    MeterComponent meters;

    NeuralAmpAudioProcessor& processor;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
};
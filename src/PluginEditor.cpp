#include "PluginEditor.h"

static juce::Colour bg1 = juce::Colour(0xff0f0f10);
static juce::Colour bg2 = juce::Colour(0xff1f1f22);

NeuralAmpAudioProcessorEditor::NeuralAmpAudioProcessorEditor (NeuralAmpAudioProcessor& p)
: AudioProcessorEditor (&p),
  gainKnob("Gain",   *p.getValueTreeState().getParameter(Params::gain), p.getValueTreeState()),
  bassKnob("Bass",   *p.getValueTreeState().getParameter(Params::bass), p.getValueTreeState()),
  midKnob("Mid",     *p.getValueTreeState().getParameter(Params::mid), p.getValueTreeState()),
  trebleKnob("Treble", *p.getValueTreeState().getParameter(Params::treble), p.getValueTreeState()),
  presenceKnob("Presence", *p.getValueTreeState().getParameter(Params::presence), p.getValueTreeState()),
  volumeKnob("Volume", *p.getValueTreeState().getParameter(Params::volume), p.getValueTreeState()),
  masterKnob("Master", *p.getValueTreeState().getParameter(Params::master), p.getValueTreeState()),
  meters([&]{ return p.getInputMeterDb(); }, [&]{ return p.getOutputMeterDb(); }),
  processor(p)
{
    setSize (820, 420);

    setLookAndFeel(&lnf);

    addAndMakeVisible(gainKnob);
    addAndMakeVisible(bassKnob);
    addAndMakeVisible(midKnob);
    addAndMakeVisible(trebleKnob);
    addAndMakeVisible(presenceKnob);
    addAndMakeVisible(volumeKnob);
    addAndMakeVisible(masterKnob);

    addAndMakeVisible(bypassButton);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        p.getValueTreeState(), Params::bypass, bypassButton);

    addAndMakeVisible(saveButton);
    addAndMakeVisible(loadButton);

    addAndMakeVisible(meters);

    saveButton.onClick = [this]
    {
        juce::FileChooser chooser("Save NeuralAmp preset", juce::File(), "*.neuralamp");
        if (chooser.browseForFileToSave(true))
        {
            auto file = chooser.getResult();
            processor.savePresetToFile(file.withFileExtension(".neuralamp"));
        }
    };

    loadButton.onClick = [this]
    {
        juce::FileChooser chooser("Load NeuralAmp preset", juce::File(), "*.neuralamp");
        if (chooser.browseForFileToOpen())
        {
            processor.loadPresetFromFile(chooser.getResult());
        }
    };

    startTimerHz(30);
}

void NeuralAmpAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient grad(bg1, 0, 0, bg2, 0, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillAll();

    auto header = juce::Rectangle<int>(0, 0, getWidth(), 40);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawFittedText("NeuralAmp - Tube Style", header, juce::Justification::centred, 1);

    auto ampPanel = juce::Rectangle<int>(10, 50, getWidth() - 20, getHeight() - 60);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(ampPanel.toFloat(), 10.0f);
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.drawRoundedRectangle(ampPanel.toFloat(), 10.0f, 1.0f);
}

void NeuralAmpAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(10);
    auto header = r.removeFromTop(40);

    auto bottom = r.removeFromBottom(80);
    auto metersArea = r.removeFromRight(160);

    meters.setBounds(metersArea);

    auto controls = r.reduced(10);
    auto row1 = controls.removeFromTop(150);
    auto row2 = controls.removeFromTop(150);

    auto knobW = row1.getWidth() / 4;
    gainKnob.setBounds(row1.removeFromLeft(knobW).reduced(8));
    bassKnob.setBounds(row1.removeFromLeft(knobW).reduced(8));
    midKnob.setBounds(row1.removeFromLeft(knobW).reduced(8));
    trebleKnob.setBounds(row1.reduced(8));

    knobW = row2.getWidth() / 4;
    presenceKnob.setBounds(row2.removeFromLeft(knobW).reduced(8));
    volumeKnob.setBounds(row2.removeFromLeft(knobW).reduced(8));
    masterKnob.setBounds(row2.removeFromLeft(knobW).reduced(8));

    auto right = row2.reduced(8);
    bypassButton.setBounds(right.removeFromTop(30));
    saveButton.setBounds(right.removeFromTop(30).reduced(0, 5));
    loadButton.setBounds(right.removeFromTop(30).reduced(0, 5));
}
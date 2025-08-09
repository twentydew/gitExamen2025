#include "PluginProcessor.h"
#include "PluginEditor.h"

NeuralAmpAudioProcessor::NeuralAmpAudioProcessor()
: juce::AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
  apvts(*this, nullptr, "Params", Params::createParameterLayout())
{
}

bool NeuralAmpAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input must match output
    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void NeuralAmpAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    audioEngine.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void NeuralAmpAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto numInputChannels = getTotalNumInputChannels();
    const auto numOutputChannels = getTotalNumOutputChannels();

    for (int ch = numInputChannels; ch < numOutputChannels; ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    auto& vts = apvts;
    auto get = [&] (const char* id) -> float { return vts.getRawParameterValue(id)->load(); };

    audioEngine.setParameters(
        get(Params::gain),
        get(Params::bass),
        get(Params::mid),
        get(Params::treble),
        get(Params::presence),
        get(Params::volume),
        get(Params::master),
        get(Params::bypass) > 0.5f
    );

    audioEngine.process(buffer);
}

juce::AudioProcessorEditor* NeuralAmpAudioProcessor::createEditor()
{
    return new NeuralAmpAudioProcessorEditor (*this);
}

void NeuralAmpAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void NeuralAmpAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

void NeuralAmpAudioProcessor::savePresetToFile(const juce::File& file)
{
    juce::MemoryBlock block;
    getStateInformation(block);
    file.replaceWithData(block.getData(), block.getSize());
}

void NeuralAmpAudioProcessor::loadPresetFromFile(const juce::File& file)
{
    juce::MemoryBlock block;
    file.loadFileAsData(block);
    setStateInformation(block.getData(), static_cast<int>(block.getSize()));
}
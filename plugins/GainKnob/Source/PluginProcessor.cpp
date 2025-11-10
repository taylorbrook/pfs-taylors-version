#include "PluginProcessor.h"
#include "PluginEditor.h"

GainKnobAudioProcessor::GainKnobAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

GainKnobAudioProcessor::~GainKnobAudioProcessor()
{
}

void GainKnobAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialization will be added in Stage 4
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void GainKnobAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 4
}

void GainKnobAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Pass-through for Stage 2 (DSP added in Stage 4)
    // Audio routing is already handled by JUCE
}

juce::AudioProcessorEditor* GainKnobAudioProcessor::createEditor()
{
    return new GainKnobAudioProcessorEditor(*this);
}

void GainKnobAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // State management will be added in Stage 3
    juce::ignoreUnused(destData);
}

void GainKnobAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // State management will be added in Stage 3
    juce::ignoreUnused(data, sizeInBytes);
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainKnobAudioProcessor();
}

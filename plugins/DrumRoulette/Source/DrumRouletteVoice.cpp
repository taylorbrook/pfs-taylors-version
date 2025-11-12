#include "DrumRouletteVoice.h"
#include <juce_audio_formats/juce_audio_formats.h>

DrumRouletteVoice::DrumRouletteVoice(int slotNum)
    : slotNumber(slotNum)
{
}

bool DrumRouletteVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<DrumRouletteSound*>(sound) != nullptr;
}

void DrumRouletteVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    juce::ignoreUnused(midiNoteNumber);

    currentPosition = 0.0;
    noteVelocity = velocity;
    isActive = true;
}

void DrumRouletteVoice::stopNote(float, bool)
{
    // Immediate stop (no envelope yet - Phase 4.2 will add release)
    isActive = false;
    clearCurrentNote();
}

void DrumRouletteVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isActive || sampleBuffer.getNumSamples() == 0)
        return;

    const int numChannels = juce::jmin(outputBuffer.getNumChannels(), sampleBuffer.getNumChannels());

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const int intPosition = static_cast<int>(currentPosition);

        if (intPosition >= sampleBuffer.getNumSamples())
        {
            // Sample finished playing
            isActive = false;
            clearCurrentNote();
            break;
        }

        // Basic sample playback with velocity scaling (no pitch shift yet - Phase 4.2)
        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float sampleValue = sampleBuffer.getSample(channel, intPosition);
            const float outputValue = sampleValue * noteVelocity;
            outputBuffer.addSample(channel, startSample + sample, outputValue);
        }

        currentPosition += 1.0;  // Fixed playback rate (no pitch shift yet)
    }
}

void DrumRouletteVoice::loadSample(const juce::File& file)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr)
    {
        const int numChannels = static_cast<int>(reader->numChannels);
        const int numSamples = static_cast<int>(reader->lengthInSamples);

        sampleBuffer.setSize(numChannels, numSamples);
        reader->read(&sampleBuffer, 0, numSamples, 0, true, true);
    }
    else
    {
        // Failed to load - clear sample buffer
        sampleBuffer.setSize(0, 0);
    }
}

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "DrumRouletteVoice.h"

class DrumRouletteAudioProcessor : public juce::AudioProcessor
{
public:
    DrumRouletteAudioProcessor();
    ~DrumRouletteAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "DrumRoulette"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void loadSampleForSlot(int slotIndex, const juce::File& file);

    juce::AudioProcessorValueTreeState parameters;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    static BusesProperties createBusesLayout();

    // DSP Components (declare BEFORE parameters for initialization order)
    juce::Synthesiser synthesiser;
    juce::AudioFormatManager formatManager;
    std::array<DrumRouletteVoice*, 8> voices;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumRouletteAudioProcessor)
};

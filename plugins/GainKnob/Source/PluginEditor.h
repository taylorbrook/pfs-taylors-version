#pragma once
#include "PluginProcessor.h"

class GainKnobAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit GainKnobAudioProcessorEditor(GainKnobAudioProcessor&);
    ~GainKnobAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    GainKnobAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainKnobAudioProcessorEditor)
};

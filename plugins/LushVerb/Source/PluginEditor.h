#pragma once
#include "PluginProcessor.h"

class LushVerbAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit LushVerbAudioProcessorEditor(LushVerbAudioProcessor&);
    ~LushVerbAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    LushVerbAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LushVerbAudioProcessorEditor)
};

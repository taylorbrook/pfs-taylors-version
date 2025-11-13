#pragma once
#include "PluginProcessor.h"

class Drum808AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit Drum808AudioProcessorEditor(Drum808AudioProcessor&);
    ~Drum808AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Drum808AudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Drum808AudioProcessorEditor)
};

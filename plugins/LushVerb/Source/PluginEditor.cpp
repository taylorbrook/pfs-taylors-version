#include "PluginEditor.h"

LushVerbAudioProcessorEditor::LushVerbAudioProcessorEditor(LushVerbAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(500, 300);
}

LushVerbAudioProcessorEditor::~LushVerbAudioProcessorEditor()
{
}

void LushVerbAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("LushVerb - Stage 2", getLocalBounds(), juce::Justification::centred, 1);
}

void LushVerbAudioProcessorEditor::resized()
{
    // Layout will be added in Stage 5
}

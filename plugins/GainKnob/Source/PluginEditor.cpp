#include "PluginEditor.h"

GainKnobAudioProcessorEditor::GainKnobAudioProcessorEditor(GainKnobAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(600, 400);
}

GainKnobAudioProcessorEditor::~GainKnobAudioProcessorEditor()
{
}

void GainKnobAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("GainKnob - Stage 2", getLocalBounds(), juce::Justification::centred, 1);
}

void GainKnobAudioProcessorEditor::resized()
{
    // Layout will be added in Stage 5
}

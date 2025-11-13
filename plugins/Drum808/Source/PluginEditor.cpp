#include "PluginEditor.h"

Drum808AudioProcessorEditor::Drum808AudioProcessorEditor(Drum808AudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1000, 550);
}

Drum808AudioProcessorEditor::~Drum808AudioProcessorEditor()
{
}

void Drum808AudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background: brushed metal (#b8b8b8 from mockup)
    g.fillAll(juce::Colour(0xffb8b8b8));

    g.setColour(juce::Colours::black);
    g.setFont(24.0f);
    g.drawFittedText("Drum808 - Stage 2 Complete", getLocalBounds(), juce::Justification::centred, 1);

    g.setFont(14.0f);
    g.drawFittedText("24 parameters implemented",
                     getLocalBounds().reduced(20).removeFromBottom(30),
                     juce::Justification::centred, 1);

    g.setFont(12.0f);
    g.drawFittedText("WebView UI will be integrated in Stage 4",
                     getLocalBounds().reduced(20).removeFromBottom(60),
                     juce::Justification::centred, 1);
}

void Drum808AudioProcessorEditor::resized()
{
    // Layout will be added in Stage 4 (GUI)
}

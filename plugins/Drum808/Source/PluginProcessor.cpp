#include "PluginProcessor.h"
#include "PluginEditor.h"

// Parameter layout creation (BEFORE constructor)
juce::AudioProcessorValueTreeState::ParameterLayout Drum808AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // KICK (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "kick_level", 1 },
        "Kick Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        80.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "kick_tone", 1 },
        "Kick Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "kick_decay", 1 },
        "Kick Decay",
        juce::NormalisableRange<float>(50.0f, 1000.0f, 1.0f),
        400.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "kick_tuning", 1 },
        "Kick Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    // LOW TOM (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lowtom_level", 1 },
        "Low Tom Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        75.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lowtom_tone", 1 },
        "Low Tom Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lowtom_decay", 1 },
        "Low Tom Decay",
        juce::NormalisableRange<float>(50.0f, 1000.0f, 1.0f),
        300.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lowtom_tuning", 1 },
        "Low Tom Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    // MID TOM (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "midtom_level", 1 },
        "Mid Tom Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        75.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "midtom_tone", 1 },
        "Mid Tom Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "midtom_decay", 1 },
        "Mid Tom Decay",
        juce::NormalisableRange<float>(50.0f, 1000.0f, 1.0f),
        250.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "midtom_tuning", 1 },
        "Mid Tom Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        5.0f,
        "st"
    ));

    // CLAP (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "clap_level", 1 },
        "Clap Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        70.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "clap_tone", 1 },
        "Clap Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "clap_snap", 1 },
        "Clap Snap",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        60.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "clap_tuning", 1 },
        "Clap Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    // CLOSED HAT (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "closedhat_level", 1 },
        "Closed Hat Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        65.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "closedhat_tone", 1 },
        "Closed Hat Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        60.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "closedhat_decay", 1 },
        "Closed Hat Decay",
        juce::NormalisableRange<float>(20.0f, 200.0f, 1.0f),
        80.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "closedhat_tuning", 1 },
        "Closed Hat Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    // OPEN HAT (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "openhat_level", 1 },
        "Open Hat Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        60.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "openhat_tone", 1 },
        "Open Hat Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        60.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "openhat_decay", 1 },
        "Open Hat Decay",
        juce::NormalisableRange<float>(100.0f, 1000.0f, 1.0f),
        500.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "openhat_tuning", 1 },
        "Open Hat Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    return layout;
}

Drum808AudioProcessor::Drum808AudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withOutput("Main", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Kick", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Low Tom", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Mid Tom", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Clap", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Closed Hat", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Open Hat", juce::AudioChannelSet::stereo(), false))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

Drum808AudioProcessor::~Drum808AudioProcessor()
{
}

void Drum808AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // DSP initialization will be added in Stage 3
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void Drum808AudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 3
}

void Drum808AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Clear all output buses (no audio input for instruments)
    buffer.clear();

    // Parameter access example (for Stage 3 DSP implementation):
    // auto* kickLevelParam = parameters.getRawParameterValue("kick_level");
    // float kickLevel = kickLevelParam->load();  // Atomic read (real-time safe)

    // MIDI processing and synthesis will be added in Stage 3
}

juce::AudioProcessorEditor* Drum808AudioProcessor::createEditor()
{
    return new Drum808AudioProcessorEditor(*this);
}

void Drum808AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Drum808AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Drum808AudioProcessor();
}

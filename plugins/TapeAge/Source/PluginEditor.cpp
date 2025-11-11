#include "PluginEditor.h"
#include "BinaryData.h"

TapeAgeAudioProcessorEditor::TapeAgeAudioProcessorEditor(TapeAgeAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)

    // Initialize relays with parameter IDs (MUST match APVTS IDs exactly)
    , driveRelay("drive")
    , ageRelay("age")
    , mixRelay("mix")

    // Initialize WebView with options
    , webView(juce::WebBrowserComponent::Options{}
        .withNativeIntegrationEnabled()  // CRITICAL: Enables JUCE JavaScript library
        .withResourceProvider([this](const auto& url) { return getResource(url); })
        .withOptionsFrom(driveRelay)     // Register each relay
        .withOptionsFrom(ageRelay)
        .withOptionsFrom(mixRelay)
    )

    // Initialize attachments (connect parameters to relays)
    , driveAttachment(*processorRef.parameters.getParameter("drive"), driveRelay)
    , ageAttachment(*processorRef.parameters.getParameter("age"), ageRelay)
    , mixAttachment(*processorRef.parameters.getParameter("mix"), mixRelay)
{
    // Add WebView to editor
    addAndMakeVisible(webView);

    // Navigate to UI
    webView.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // Set editor size to match mockup dimensions (500x450 from v3-ui.html)
    setSize(500, 450);

    // Phase 5.2: Start timer for VU meter updates (30 FPS)
    startTimerHz(30);
}

TapeAgeAudioProcessorEditor::~TapeAgeAudioProcessorEditor()
{
    // Phase 5.2: Stop timer
    stopTimer();
}

void TapeAgeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView handles all painting
    juce::ignoreUnused(g);
}

void TapeAgeAudioProcessorEditor::resized()
{
    // WebView fills entire editor
    webView.setBounds(getLocalBounds());
}

void TapeAgeAudioProcessorEditor::timerCallback()
{
    // Phase 5.2: Send VU meter updates to JavaScript
    // Read peak level from audio processor (atomic, thread-safe)
    float dbLevel = processorRef.outputLevel.load(std::memory_order_relaxed);

    // Emit event to JavaScript (only if WebView is visible)
    webView.emitEventIfBrowserIsVisible("updateVUMeter", dbLevel);
}

std::optional<juce::WebBrowserComponent::Resource>
TapeAgeAudioProcessorEditor::getResource(const juce::String& url)
{
    // Map URLs to embedded resources
    auto resource = url.replaceCharacter('\\', '/');

    // Root "/" → index.html
    if (resource == "/" || resource.isEmpty())
        resource = "/index.html";

    // Remove leading slash for BinaryData lookup
    auto path = resource.substring(1);

    // Find in binary data (files embedded from ui/public/)
    for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
    {
        if (path == BinaryData::namedResourceList[i])
        {
            int dataSize = 0;
            const char* data = BinaryData::getNamedResource(
                BinaryData::namedResourceList[i], dataSize);

            // Determine MIME type
            juce::String mimeType = "text/html";
            if (path.endsWith(".css")) mimeType = "text/css";
            if (path.endsWith(".js")) mimeType = "application/javascript";
            if (path.endsWith(".png")) mimeType = "image/png";
            if (path.endsWith(".jpg") || path.endsWith(".jpeg")) mimeType = "image/jpeg";
            if (path.endsWith(".svg")) mimeType = "image/svg+xml";

            // Convert raw pointer to std::vector<std::byte>
            std::vector<std::byte> resourceData;
            resourceData.reserve(static_cast<size_t>(dataSize));
            for (int j = 0; j < dataSize; ++j)
                resourceData.push_back(static_cast<std::byte>(data[j]));

            return juce::WebBrowserComponent::Resource{
                std::move(resourceData), mimeType
            };
        }
    }

    // Resource not found
    juce::Logger::writeToLog("Resource not found: " + url);
    return std::nullopt;
}

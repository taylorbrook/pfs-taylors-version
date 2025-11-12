cmake_minimum_required(VERSION 3.15)

# Plugin formats: VST3, AU, Standalone
juce_add_plugin(LushVerb
    COMPANY_NAME "PluginFreedom"
    PLUGIN_MANUFACTURER_CODE PfCo
    PLUGIN_CODE Lush
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "LushVerb"
    NEEDS_WEB_BROWSER TRUE
)

# Source files (minimal for Stage 2)
target_sources(LushVerb
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Include paths
target_include_directories(LushVerb
    PRIVATE
        Source
)

# Required JUCE modules
target_link_libraries(LushVerb
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Generate JuceHeader.h (JUCE 8 requirement - CRITICAL: Pattern #1)
juce_generate_juce_header(LushVerb)

# Compile definitions
target_compile_definitions(LushVerb
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# WebView support (CRITICAL: Pattern #9)
target_compile_definitions(LushVerb
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
)

# WebView UI Resources (CRITICAL: Pattern #8, #13)
juce_add_binary_data(LushVerb_UIResources
    SOURCES
        Source/ui/public/index.html
        Source/ui/public/js/juce/index.js
        Source/ui/public/js/juce/check_native_interop.js
)

target_link_libraries(LushVerb
    PRIVATE
        LushVerb_UIResources
)

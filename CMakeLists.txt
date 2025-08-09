cmake_minimum_required(VERSION 3.21)

project(NeuralAmp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optionally allow user to point to an existing JUCE checkout
option(NEURAL_AMP_USE_SYSTEM_JUCE "Use a pre-installed JUCE (set JUCE_DIR) instead of FetchContent" OFF)

include(FetchContent)

if(NEURAL_AMP_USE_SYSTEM_JUCE)
  if(NOT DEFINED JUCE_DIR)
    message(FATAL_ERROR "NEURAL_AMP_USE_SYSTEM_JUCE=ON but JUCE_DIR is not set")
  endif()
  add_subdirectory(${JUCE_DIR} juce)
else()
  FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.3
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(juce)
endif()

# Enable LTO if available for better performance
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_output)
if(lto_supported)
  message(STATUS "IPO/LTO enabled")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif()

# Collect sources
file(GLOB_RECURSE NEURAL_AMP_SOURCES
  CONFIGURE_DEPENDS
  src/*.cpp
  src/*.h
)

# Plugin target definition
juce_add_plugin(NeuralAmp
  PRODUCT_NAME "NeuralAmp"
  COMPANY_NAME "NeuralDSPStyle"
  BUNDLE_ID com.example.neuralamp
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
  PLUGIN_CODE NAMP
  VST3_CATEGORIES Fx
  FORMATS VST3 AU Standalone
  AU_MAIN_TYPE kAudioUnitType_Effect
)

# Common sources and libraries for all variants of the plugin (VST3 and Standalone)
target_sources(NeuralAmp PRIVATE ${NEURAL_AMP_SOURCES})

target_compile_definitions(NeuralAmp PRIVATE
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
)

if(WIN32)
  target_compile_definitions(NeuralAmp PRIVATE JUCE_USE_ASIO=1)
endif()

# Link JUCE modules
target_link_libraries(NeuralAmp PRIVATE
  juce::juce_audio_utils
  juce::juce_audio_processors
  juce::juce_dsp
  juce::juce_gui_extra
  juce::juce_core
  juce::juce_graphics
)

# Group source files nicely in IDEs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source" FILES ${NEURAL_AMP_SOURCES})

# Install rules (plugins get copied after build; additionally we can install standalone)
install(TARGETS NeuralAmp RUNTIME DESTINATION bin)
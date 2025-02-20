cmake_minimum_required(VERSION 3.26.4)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/lib)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE)

add_subdirectory(engine)
if( BUILD_INTEGRATION_TESTS )
    add_subdirectory(tests/integration)
endif()


project(Autotuner)

if(LINUX)
    include_directories(
        /usr/include/X11
        /usr/include/gtk-3.0
        /usr/include/glib-2.0
        /usr/lib/x86_64-linux-gnu/glib-2.0/include
        /usr/include/cairo
        /usr/include/pango-1.0
        /usr/include/harfbuzz
        /usr/lib/x86_64-linux-gnu/gtk-2.0/include
        /usr/include/gdk-pixbuf-2.0/
        /usr/include/atk-1.0
        /usr/include/webkitgtk-4.1
        /usr/include/libsoup-3.0
    )
    link_directories(
        /usr/lib/x86_64-linux-gnu
    )
endif()

find_package(CURL REQUIRED)

set(SUPPRESS_WARNINGS "-w")

if(MSVC)
    set(SUPPRESS_WARNINGS "/W0")
endif()

add_subdirectory(dependencies/JUCE)
set_source_files_properties(DIRECTORY dependencies/JUCE/ PROPERTIES COMPILE_FLAGS ${SUPPRESS_WARNINGS})

link_libraries(${CURL_LIBRARIES})

juce_add_plugin(Autotuner
    PLUGIN_NAME "Autotuner"
    VERSION "0.0.1"
    FORMATS VST3 AU Standalone
    PLUGIN_MANUFACTURER_CODE WILL
    PLUGIN_CODE TEST

    PRODUCT_NAME "Willi Autotuner"
    COMPANY_NAME "Willi"
    BUNDLE_ID com.willi.autotuner
    VST3_CATEGORIES Spatial
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
)

juce_generate_juce_header(Autotuner)
target_sources(Autotuner PRIVATE
    frontend/source/Main.cpp
    frontend/source/Processor.cpp
)

target_compile_options(Autotuner PRIVATE
    ${FRONTEND_COMPILER_FLAGS}
)

target_include_directories(Autotuner PRIVATE 
    ${CURL_INCLUDE_DIRS}
    frontend/include
    engine/include
)

target_link_libraries(Autotuner PRIVATE
        juce::juce_core
        juce::juce_audio_utils
        juce::juce_analytics
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        AutotunerEngine
    )

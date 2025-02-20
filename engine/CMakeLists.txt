project(AutotunerEngine)

find_package(kissfft REQUIRED)

add_library(AutotunerEngine STATIC)

set(AUTOTUNER_ENGINE_SOURCES
    source/CircularAudioBuffer.cpp
    source/AudioBuffer.cpp
    source/PeriodDetector.cpp
    source/GoalPeriodCalculator.cpp
    source/PitchBender.cpp
    source/NormalizedSquareDifferenceCalculator.cpp
    source/ValidPeriodCalculator.cpp
    source/Backend.cpp
)

target_sources(AutotunerEngine PRIVATE ${AUTOTUNER_ENGINE_SOURCES})

if(WIN32)
    set_target_properties(AutotunerEngine PROPERTIES DEBUG_POSTFIX d)
endif()

target_include_directories(AutotunerEngine
    PUBLIC
        include
        )

target_compile_features(AutotunerEngine PUBLIC cxx_std_17)

target_compile_options(AutotunerEngine
        PRIVATE
        ${BACKEND_COMPILER_FLAGS}
        $<$<CXX_COMPILER_ID:GNU>: -fPIC>
        )

target_link_libraries(AutotunerEngine
    PUBLIC
        kissfft::kissfft
        )


project(autotunerIntegrationTestHost)
include(GoogleTest)
include(CTest)

find_package(GTest REQUIRED)

add_executable(autotunerIntegrationTestHost
   source/GoalPeriodCalculatorTestCase.cpp
    source/AudioBufferTestCase.cpp
    source/CircularAudioBufferTestCase.cpp
    source/PeriodDetectorTestCase.cpp
    source/PitchBenderTestCase.cpp
   source/NormalizedSquareDifferenceCalculatorTestCase.cpp
)

target_include_directories(autotunerIntegrationTestHost PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(autotunerIntegrationTestHost PRIVATE cxx_std_17)

target_compile_options(autotunerIntegrationTestHost
        PRIVATE
        ${BACKEND_COMPILER_FLAGS}
        )

target_link_libraries(autotunerIntegrationTestHost
    PRIVATE
        GTest::gtest
        GTest::gmock_main
        GTest::gmock
        AutotunerEngine
        )

gtest_discover_tests(autotunerIntegrationTestHost)



if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "OS X deployment target" FORCE)

    set(osx_arch_arm64 FALSE)
    foreach(osx_arch ${CMAKE_OSX_ARCHITECTURES})
        if("${osx_arch}" STREQUAL "arm64")
            set(osx_arch_arm64 TRUE)
        endif()
    endforeach()

    set(BACKEND_COMPILER_FLAGS
            -Werror
            -Wall
            -Wextra
            -Wshadow-all
            -Wpedantic
            -Wnullable-to-nonnull-conversion
            -Wuninitialized 
            -Wconversion
            -Wno-sign-conversion 
            -Wconditional-uninitialized
            -Wextra-semi 
            -Wunreachable-code
            -Wcast-align 
            -Wshift-sign-overflow
            -Wswitch-enum
            -Wzero-as-null-pointer-constant
            -Wunused-private-field
            -Woverloaded-virtual
            -Wreorder
            -Winconsistent-missing-destructor-override

            -Wno-gnu-zero-variadic-macro-arguments
            -Wno-empty-translation-unit

            $<$<CONFIG:Release>:-DNDEBUG>
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:Release>:-flto>
            $<$<CONFIG:Release>:-fstrict-aliasing>
            $<$<CONFIG:Release>:-fasm-blocks>
    )
    set(FRONTEND_COMPILER_FLAGS
            -Wnullable-to-nonnull-conversion
            -Wuninitialized 
            -Wconversion
            -Wno-sign-conversion 
            -Wconditional-uninitialized
            -Wextra-semi 
            -Wunreachable-code
            -Wcast-align 
            -Wshift-sign-overflow
            -Wswitch-enum
            -Wzero-as-null-pointer-constant
            -Wunused-private-field
            -Woverloaded-virtual
            -Wreorder
            -Winconsistent-missing-destructor-override

            -Wno-gnu-zero-variadic-macro-arguments
            -Wno-empty-translation-unit

            $<$<CONFIG:Release>:-DNDEBUG>
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:Release>:-flto>
            $<$<CONFIG:Release>:-fstrict-aliasing>
            $<$<CONFIG:Release>:-fasm-blocks>
    )



    if(NOT osx_arch_arm64)
        add_compile_options(-mxsave)
    endif()

    add_link_options(
            -stdlib=libc++
            $<$<CONFIG:Release>:-flto>
            $<$<CONFIG:Release>:-dead_strip>
    )
elseif (WIN32)
    set(BACKEND_COMPILER_FLAGS
        /GR /EHsc /MP /Wall /W4 /WX /Zc:inline
        $<$<CONFIG:Release>:/MD>
        $<$<CONFIG:Release>:/Ob2>
        $<$<CONFIG:Release>:/DNDEBUG>

        /wd4206
        /D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    )

    set(FRONTEND_COMPILER_FLAGS
        /GR /EHsc /MP /W0 /Zc:inline
        $<$<CONFIG:Release>:/MD>
        $<$<CONFIG:Release>:/Ob2>
        $<$<CONFIG:Release>:/DNDEBUG>

        /wd4206
        /D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    )
else()
    set(BACKEND_COMPILER_FLAGS
        -Wall
        -Wextra
        -Werror
        -Wuninitialized
        -Winit-self
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wcast-align
        -Wswitch-enum
        -Wunused
        -mavx2
        -msse3

        $<$<CONFIG:Release>:-DNDEBUG>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-fstrict-aliasing>
    )

    set(FRONTEND_COMPILER_FLAGS
        -mavx2
        -msse3

        $<$<CONFIG:Release>:-DNDEBUG>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-fstrict-aliasing>
    )
endif ()

add_subdirectory(${PROJECT_SOURCE_DIR}/../core codspeed)

execute_process(
    COMMAND git rev-parse --show-toplevel
    OUTPUT_VARIABLE GIT_ROOT_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_COMMAND_RESULT
)

if(NOT GIT_COMMAND_RESULT EQUAL 0)
    message(
        WARNING
        "Failed to determine the git root directory.\
        Check that git is in your PATH, and that you are in a git repository.\
        Continuing, but codspeed features will not be useable"
    )
    # Default to user's cmake source directory
    set(GIT_ROOT_DIR ${CMAKE_SOURCE_DIR})
endif()

target_compile_definitions(
    codspeed
    INTERFACE -DCODSPEED_GIT_ROOT_DIR="${GIT_ROOT_DIR}"
)

if(DEFINED CODSPEED_MODE)
    # Define a preprocessor macro based on the build mode
    if(CODSPEED_MODE STREQUAL "instrumentation")
        target_compile_definitions(
            codspeed
            INTERFACE -DCODSPEED_INSTRUMENTATION
        )
    elseif(CODSPEED_MODE STREQUAL "walltime")
        target_compile_definitions(codspeed INTERFACE -DCODSPEED_WALLTIME)
    else()
        message(
            FATAL_ERROR
            "Invalid build mode: ${CODSPEED_MODE}. Use 'instrumentation' or 'walltime'."
        )
    endif()
endif()

message(STATUS "Build mode set to: ${CODSPEED_MODE}")

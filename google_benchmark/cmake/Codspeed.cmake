# Step 1: Check if CODSPEED_MODE is set via the command line

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

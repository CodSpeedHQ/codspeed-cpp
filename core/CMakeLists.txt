cmake_minimum_required(VERSION 3.10)

set(CODSPEED_VERSION 1.2.0)

project(codspeed VERSION ${CODSPEED_VERSION} LANGUAGES CXX C)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory
include_directories(include)

# Add the instrument_hooks library
add_library(
    instrument_hooks
    STATIC
    instrument-hooks/dist/core.c
)

target_include_directories(
    instrument_hooks
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/instrument-hooks/includes>
    $<INSTALL_INTERFACE:includes>
)

# Suppress warnings for the instrument_hooks library
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        instrument_hooks
        PRIVATE
        -Wno-maybe-uninitialized
        -Wno-unused-variable
        -Wno-unused-parameter
        -Wno-unused-but-set-variable
        -Wno-type-limits
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(
        instrument_hooks
        PRIVATE
        /wd4101  # unreferenced local variable (equivalent to -Wno-unused-variable)
        /wd4189  # local variable is initialized but not referenced (equivalent to -Wno-unused-but-set-variable)
        /wd4100  # unreferenced formal parameter (equivalent to -Wno-unused-parameter)
        /wd4245  # signed/unsigned mismatch
        /wd4132  # const object should be initialized
        /wd4146  # unary minus operator applied to unsigned type
    )
endif()


# Add the main library
add_library(
    codspeed
    src/codspeed.cpp
    src/measurement.cpp
    src/walltime.cpp
    src/uri.cpp
    src/workspace.cpp
)

# Link instrument_hooks to codspeed
target_link_libraries(codspeed PRIVATE instrument_hooks)

# Version
add_compile_definitions(CODSPEED_VERSION="${CODSPEED_VERSION}")

# Specify the include directories for users of the library
target_include_directories(
    codspeed
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/instrument-hooks/includes>
)

# Disable valgrind compilation errors
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Disable the old-style-cast warning for the specific target
    target_compile_options(codspeed INTERFACE -Wno-old-style-cast)
endif()

# Allow the user to override CODSPEED_ROOT_DIR from the command line
set(CODSPEED_ROOT_DIR "" CACHE PATH "Root directory for Codspeed")

if(NOT CODSPEED_ROOT_DIR)
    execute_process(
        COMMAND git rev-parse --show-toplevel
        OUTPUT_VARIABLE CODSPEED_ROOT_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_COMMAND_RESULT
    )

    if(GIT_COMMAND_RESULT EQUAL 0)
        message(STATUS "Detected Codspeed root directory: ${CODSPEED_ROOT_DIR}")
    else()
        message(
            WARNING
            "CODSPEED_ROOT_DIR will default to PWD at runtime as git detection failed."
        )
    endif()
else()
    message(
        STATUS
        "Using user-specified Codspeed root directory: ${CODSPEED_ROOT_DIR}"
    )
endif()

# Define the CODSPEED_ROOT_DIR macro for the target
target_compile_definitions(
    codspeed
    PRIVATE -DCODSPEED_ROOT_DIR="${CODSPEED_ROOT_DIR}"
)

message(STATUS "Using codspeed root directory: ${CODSPEED_ROOT_DIR}")

set(CODSPEED_MODE_ALLOWED_VALUES "off" "instrumentation" "walltime")
set(CODSPEED_MODE "off" CACHE STRING "Build mode for Codspeed")
set_property(
    CACHE CODSPEED_MODE
    PROPERTY STRINGS ${CODSPEED_MODE_ALLOWED_VALUES}
)

if(NOT CODSPEED_MODE STREQUAL "off")
    target_compile_definitions(codspeed PUBLIC -DCODSPEED_ENABLED)

    if(NOT CMAKE_BUILD_TYPE)
        message(
            WARNING
            "CMAKE_BUILD_TYPE is not set. Consider setting it to 'RelWithDebInfo'."
        )
    elseif(NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        message(
            WARNING
            "CMAKE_BUILD_TYPE is set to '${CMAKE_BUILD_TYPE}', but 'RelWithDebInfo' is recommended."
        )
    endif()

    # Define a preprocessor macro based on the build mode
    if(CODSPEED_MODE STREQUAL "instrumentation")
        target_compile_definitions(
            codspeed
            PUBLIC -DCODSPEED_INSTRUMENTATION
        )
    elseif(CODSPEED_MODE STREQUAL "walltime")
        target_compile_definitions(codspeed PUBLIC -DCODSPEED_WALLTIME)
    else()
        message(
            FATAL_ERROR
            "Invalid build mode: ${CODSPEED_MODE}. Use one of ${CODSPEED_MODE_ALLOWED_VALUES}."
        )
    endif()
endif()

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include"
    DESTINATION /usr/local
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)

install(
    TARGETS codspeed instrument_hooks
    EXPORT codspeed-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

message(STATUS "Codspeed mode: ${CODSPEED_MODE}")

option(ENABLE_TESTS "Enable building the unit tests which depend on gtest" OFF)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
  uri.cpp
  codspeed.cpp
  walltime.cpp
)

target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(unit_tests
  PRIVATE
  codspeed
  GTest::gtest
  GTest::gtest_main
)

# Treat warnings as errors for tests to catch issues early
if(MSVC)
  target_compile_options(unit_tests PRIVATE /W4 /WX)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(unit_tests PRIVATE -Wall -Wextra -Werror)
endif()

include(GoogleTest)
gtest_discover_tests(unit_tests)

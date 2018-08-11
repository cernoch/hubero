cmake_minimum_required(VERSION 3.2)
project(boolSATlib LANGUAGES CXX VERSION 0.0.1)

# Load coverage-related stuff
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/misc/cmake")
if(BOOLSATLIB_ENABLE_COVERAGE)
    set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
    find_package(codecov)
    list(APPEND LCOV_REMOVE_PATTERNS "/usr/*")
endif()

# Force C++11, also disable GNU extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# List sources
set(BOOLSATLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BOOLSATLIB_LIB_DIR ${BOOLSATLIB_DIR}/boolSATlib)
set(BOOLSATLIB_CLI_DIR ${BOOLSATLIB_DIR}/clitools)
set(BOOLSATLIB_TEST_DIR ${BOOLSATLIB_DIR}/tests)

set(BOOLSATLIB_LIB_FILES
    ${BOOLSATLIB_LIB_DIR}/core.hpp
)

set(BOOLSATLIB_TEST_FILES
    ${BOOLSATLIB_TEST_DIR}/core_lit_test.cpp
    ${BOOLSATLIB_TEST_DIR}/core_var_test.cpp
    ${BOOLSATLIB_TEST_DIR}/tools_test.cpp
)

# Use folders to group files when generating project files
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group("Library" FILES ${BOOLSATLIB_LIB_DIR})
source_group("CLI" FILES ${BOOLSATLIB_CLI_DIR})
source_group("Tests" FILES ${BOOLSATLIB_TEST_DIR})

# Target: The Library
add_library(boolSATlib INTERFACE)
target_include_directories(boolSATlib INTERFACE
    $<BUILD_INTERFACE:${BOOLSATLIB_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Target: Unit tests
add_executable(bs-tests
    ${BOOLSATLIB_TEST_DIR}/catch_main.cpp
    ${BOOLSATLIB_TEST_FILES}
)

target_compile_definitions(bs-tests PRIVATE
    CATCH_CONFIG_FAST_COMPILE
    $<$<PLATFORM_ID:Windows>:CATCH_CONFIG_WINDOWS_CRTDBG>
)

# Target: Executable files
add_executable(bs-main ${BOOLSATLIB_CLI_DIR}/main.cpp)
set(BOOLSATLIB_BINARIES
    bs-main
)

foreach(target bs-tests ${BOOLSATLIB_BINARIES})
    target_link_libraries(${target} boolSATlib)

    # Enable coverage reporting
    if (BOOLSATLIB_ENABLE_COVERAGE)
        add_coverage(${target})
    endif()

    # Compiler-specific flags
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(${target} PUBLIC
        -Wall -Wextra $<$<CXX_COMPILER_ID:Clang>:-Wmissing-noreturn>
    )
    endif()
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${target} PUBLIC /W4 /wd4800 /wd4200 /wd4245 /w44265 /w44061 /w44062 /WX)
    endif()
endforeach()

if(BOOLSATLIB_ENABLE_COVERAGE)
    coverage_evaluate()
endif()

### Enable make test and ctest
set(CATCH_FLAGS -r compact -\# ~[.])
include(CTest)

# We compile one large binary, but use Catch's -# flag to group tests by their implementation file
# and register multiple ctest tests (for more granularity and parallelization opportunities)
foreach(test_file ${BOOLSATLIB_TEST_FILES})
    get_filename_component(no_ext ${test_file} NAME_WE)
    if( no_ext MATCHES "_test")
        add_test(NAME ${no_ext} COMMAND bs-tests -r compact -\# ~[.] [\#${no_ext}])
        set_tests_properties(${no_ext} PROPERTIES FAIL_REGULAR_EXPRESSION "No tests ran.")
        set_tests_properties(${no_ext} PROPERTIES TIMEOUT 500)
    endif()
endforeach()

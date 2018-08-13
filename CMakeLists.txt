cmake_minimum_required(VERSION 3.2)
project(hubero LANGUAGES CXX VERSION 0.0.1)

# Load coverage-related stuff
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/misc/cmake")
if(HUBERO_ENABLE_COVERAGE)
    set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
    find_package(codecov)
    list(APPEND LCOV_REMOVE_PATTERNS "/usr/*")
endif()

# Disable GNU extensions
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# List sources
set(HUBERO_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(HUBERO_LIB_DIR ${HUBERO_DIR}/src/hubero)
set(HUBERO_CLI_DIR ${HUBERO_DIR}/clitools)
set(HUBERO_TEST_DIR ${HUBERO_DIR}/tests)

set(HUBERO_LIB_FILES
    ${HUBERO_LIB_DIR}/core.hpp
)

set(HUBERO_TEST_FILES
    ${HUBERO_TEST_DIR}/core_dimacs_lit_test.cpp
    ${HUBERO_TEST_DIR}/core_mini_lit_test.cpp
    ${HUBERO_TEST_DIR}/core_var_test.cpp
    ${HUBERO_TEST_DIR}/tools_test.cpp
)

# Use folders to group files when generating project files
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group("Library" FILES ${HUBERO_LIB_DIR})
source_group("CLI" FILES ${HUBERO_CLI_DIR})
source_group("Tests" FILES ${HUBERO_TEST_DIR})

# Target: The Library
add_library(hubero INTERFACE)
target_include_directories(hubero INTERFACE
    $<BUILD_INTERFACE:${HUBERO_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
# Effectively set C++11 (at least)
target_compile_features(hubero
  INTERFACE
    cxx_alignas
    cxx_alignof
    cxx_attributes
    cxx_auto_type
    cxx_constexpr
    cxx_defaulted_functions
    cxx_deleted_functions
    cxx_final
    cxx_lambdas
    cxx_noexcept
    cxx_override
    cxx_range_for
    cxx_rvalue_references
    cxx_static_assert
    cxx_strong_enums
    cxx_trailing_return_types
    cxx_unicode_literals
    cxx_user_literals
    cxx_variadic_macros
)

# Target: Unit tests
add_executable(hubero-tests
    ${HUBERO_TEST_DIR}/catch_main.cpp
    ${HUBERO_TEST_FILES}
)

target_compile_definitions(hubero-tests PRIVATE
    CATCH_CONFIG_FAST_COMPILE
    $<$<PLATFORM_ID:Windows>:CATCH_CONFIG_WINDOWS_CRTDBG>
)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # STL iterators are checked on Linux
    target_compile_definitions(hubero-tests PRIVATE
        $<$<PLATFORM_ID:Linux>:_HAS_ITERATOR_DEBUGGING>
    )
endif()

# Target: Executable files
add_executable(hubero-main ${HUBERO_CLI_DIR}/main.cpp)
set(HUBERO_BINARIES
    hubero-main
)

foreach(target hubero-tests ${HUBERO_BINARIES})
    target_link_libraries(${target} hubero)

    # Enable coverage reporting
    if (HUBERO_ENABLE_COVERAGE)
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

if(HUBERO_ENABLE_COVERAGE)
    coverage_evaluate()
endif()

### Enable make test and ctest
set(CATCH_FLAGS -r compact -\# ~[.])
include(CTest)

# We compile one large binary, but use Catch's -# flag to group tests by their implementation file
# and register multiple ctest tests (for more granularity and parallelization opportunities)
foreach(test_file ${HUBERO_TEST_FILES})
    get_filename_component(no_ext ${test_file} NAME_WE)
    if( no_ext MATCHES "_test")
        add_test(NAME ${no_ext} COMMAND hubero-tests -r compact -\# ~[.] [\#${no_ext}])
        set_tests_properties(${no_ext} PROPERTIES FAIL_REGULAR_EXPRESSION "No tests ran.")
        set_tests_properties(${no_ext} PROPERTIES TIMEOUT 500)
    endif()
endforeach()

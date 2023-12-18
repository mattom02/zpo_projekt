cmake_minimum_required(VERSION 3.24)
project(netsim)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Wconversion)

include_directories(
        include
)

set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        src/nodes.cpp
        src/helpers.cpp
        src/factory.cpp
        src/reports.cpp
        src/simulation.cpp
        )

set(SOURCES_FILES_TESTS
        test/main_gtest.cpp
        test/test_package.cpp
        test/test_storage_types.cpp
        test/test_nodes.cpp
        test/test_Factory.cpp
        test/test_factory_io.cpp
        test/test_reports.cpp
        test/test_simulate.cpp
        )

add_executable(netsim ${SOURCE_FILES} main.cpp)

add_executable(netsim_test ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)

target_compile_definitions(netsim_test PUBLIC EXERCISE_ID=SIMULATION)

target_include_directories(netsim_test PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks
        )

add_subdirectory(googletest-master)

target_link_libraries(netsim_test gmock)
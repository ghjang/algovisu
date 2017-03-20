cmake_minimum_required(VERSION 3.0.0)
project(algovisu_test VERSION 0.0.0)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(BOOST_ROOT "../../boost_1_63_0")
find_package(Boost 1.63.0)
if(Boost_FOUND)
    include_directories(${BOOST_ROOT})
else()
    message(FATAL_ERROR "Could not find boost!")
endif()

set(ALGOVISU_ROOT "../src")
include_directories(${ALGOVISU_ROOT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

set(SOURCE_FILES
        spirit_lex_test.cpp
        calculator_test.cpp
        main.cpp)

add_executable(algovisu_test ${SOURCE_FILES})

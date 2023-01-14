cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(QDmxLib VERSION 1.0.0 LANGUAGES C CXX)

if(APPLE)
    enable_language(OBJCXX)
endif()

set(QDMXLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE GIT_VERSION
)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED QDMXLIB_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(QDMXLIB_MASTER_PROJECT ON)
    else()
        set(QDMXLIB_MASTER_PROJECT OFF)
    endif()
endif()

if(NOT TARGET Qt::Core)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
    set(Qt "Qt${QT_VERSION_MAJOR}")

    find_package(${Qt} COMPONENTS Core REQUIRED)
    find_package(${Qt} COMPONENTS Network REQUIRED)
    find_package(${Qt} COMPONENTS SerialPort)
endif()

find_package(PkgConfig QUIET)

find_package(LibFTDI1 QUIET)
if(NOT ${LibFTDI1_FOUND} AND ${PkgConfig_FOUND})
    pkg_search_module(LibFTDI1 libftdi1)
endif()

option(QDMXLIB_BUILD_TESTS "Build the unit tests"  ON)
option(QDMXLIB_INSTALL     "Generate install targets" ${NUMPYCPP_MASTER_PROJECT})

if(${QDMXLIB_BUILD_TESTS})
    find_package(${Qt} COMPONENTS Test REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()

set(BUILD_INCLUDE "${QDMXLIB_SOURCE_DIR}/include")

#install var
set(INCLUDE_DEST "include/qdmxlib-${QDMXLIB_VERSION}")
set(LIB_DEST "lib/qdmxlib-${QDMXLIB_VERSION}")

# add subdirs
add_subdirectory(src)

# install
if(${QDMXLIB_INSTALL})
    install(FILES qdmxlib-config.cmake DESTINATION ${LIB_DEST})
    install(EXPORT qdmxlib DESTINATION ${LIB_DEST})
endif()

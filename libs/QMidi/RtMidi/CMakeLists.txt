# Set minimum CMake required version for this project.
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Define a C++ project.
project(RtMidi LANGUAGES CXX C)

# standards version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check for Jack (any OS)
find_library(JACK_LIB jack)
find_package(PkgConfig)
pkg_check_modules(jack jack)
if(JACK_LIB OR jack_FOUND)
  set(HAVE_JACK TRUE)
endif()

# Necessary for Windows
if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Standard CMake options
option(BUILD_SHARED_LIBS "Build as shared library" OFF)

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")
endif()
if(WINDOWS)
  set(CMAKE_DEBUG_POSTFIX d CACHE STRING "Postfix for debug version of library")
endif()

# API Options
option(RTMIDI_API_JACK "Compile with JACK support." ${HAVE_JACK})
if(UNIX AND NOT APPLE)
  option(RTMIDI_API_ALSA "Compile with ALSA support." ON)
endif()
option(RTMIDI_API_WINMM "Compile with WINMM support." ${WIN32})
option(RTMIDI_API_CORE "Compile with CoreMIDI support." ${APPLE})

# Add -Wall if possible
if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (CMAKE_COMPILER_IS_GNUCXX)

# Add debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D__RTMIDI_DEBUG__)
  if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif (CMAKE_COMPILER_IS_GNUCXX)
endif ()


# Init variables
set(rtmidi_SOURCES RtMidi.cpp RtMidi.h #[[rtmidi_c.cpp rtmidi_c.h]])
set(LINKLIBS)
set(PUBLICLINKLIBS)
set(INCDIRS)
set(PKGCONFIG_REQUIRES)
set(API_DEFS)
set(API_LIST)

# Tweak API-specific configuration.

# Jack
if(RTMIDI_API_JACK)
  if (NOT HAVE_JACK)
    message(FATAL_ERROR "Jack API requested but no Jack dev libraries found")
  endif()
  set(NEED_PTHREAD ON)
  list(APPEND PKGCONFIG_REQUIRES "jack")
  list(APPEND API_DEFS "-D__UNIX_JACK__")
  list(APPEND API_LIST "jack")
  if(jack_FOUND)
    list(APPEND LINKLIBS ${jack_LIBRARIES})
    list(APPEND INCDIRS ${jack_INCLUDEDIR})
  else()
    list(APPEND LINKLIBS ${JACK_LIB})
  endif()

  # Check for jack_port_rename
  include(CheckSymbolExists)
  set(tmp_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
  list(APPEND CMAKE_REQUIRED_LIBRARIES jack)
  check_symbol_exists(jack_port_rename ${jack_INCLUDEDIR}/jack/jack.h JACK_HAS_PORT_RENAME)
  set(CMAKE_REQUIRED_LIBRARIES ${tmp_CMAKE_REQUIRED_LIBRARIES})
  if(JACK_HAS_PORT_RENAME)
    list(APPEND API_DEFS "-DJACK_HAS_PORT_RENAME")
  endif()
endif()

# ALSA
if(RTMIDI_API_ALSA)
  set(NEED_PTHREAD ON)
  find_package(ALSA)
  if (NOT ALSA_FOUND)
    message(FATAL_ERROR "ALSA API requested but no ALSA dev libraries found")
  endif()
  list(APPEND INCDIRS ${ALSA_INCLUDE_DIR})
  list(APPEND LINKLIBS ${ALSA_LIBRARY})
  list(APPEND PKGCONFIG_REQUIRES "alsa")
  list(APPEND API_DEFS "-D__LINUX_ALSA__")
  list(APPEND API_LIST "alsa")
endif()

# WinMM
if(RTMIDI_API_WINMM)
  list(APPEND API_DEFS "-D__WINDOWS_MM__")
  list(APPEND API_LIST "winmm")
  list(APPEND LINKLIBS winmm)
endif()

# CoreMIDI
if(RTMIDI_API_CORE)
  list(APPEND API_DEFS "-D__MACOSX_CORE__")
  list(APPEND API_LIST "coremidi")
  list(APPEND LINKLIBS "-framework CoreServices")
  list(APPEND LINKLIBS "-framework CoreAudio")
  list(APPEND LINKLIBS "-framework CoreMIDI")
  list(APPEND LINKLIBS "-framework CoreFoundation")
  list(APPEND LINKFLAGS "-Wl,-F/Library/Frameworks")
endif()

# pthread
if (NEED_PTHREAD)
  find_package(Threads REQUIRED
    CMAKE_THREAD_PREFER_PTHREAD
    THREADS_PREFER_PTHREAD_FLAG)
  list(APPEND PUBLICLINKLIBS Threads::Threads)
endif()


# Message
string(REPLACE ";" " " apilist "${API_LIST}")
message(STATUS "Compiling with support for: ${apilist}")

add_library(RtMidi STATIC RtMidi.cpp)

target_compile_definitions(RtMidi PRIVATE ${API_DEFS})

target_include_directories(RtMidi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RtMidi PUBLIC ${PUBLICLINKLIBS}
                             PRIVATE ${LINKLIBS})

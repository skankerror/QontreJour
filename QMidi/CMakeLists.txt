cmake_minimum_required(VERSION 3.18)

project(QMidi LANGUAGES CXX VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# pour passer numéro de version au prog
configure_file(qmidi.h.in qmidi.h)

add_subdirectory(RtMidi)

find_package(QT NAMES Qt6 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES QMidi_fr_GF.ts)

add_library(QMidi STATIC
  qmidi.h
  qmidimessage.cpp
  qmidimessage.h
  qmidiin.cpp
  qmidiin.h
  qmidiout.cpp
  qmidiout.h
  qmidipianorollview.cpp
  qmidipianorollview.h
  qabstractmidicontrolerwidget.cpp
  qabstractmidicontrolerwidget.h
  qakaiapcminiwidget.cpp
  qakaiapcminiwidget.h
  qmidiconnectwidget.cpp
  qmidiconnectwidget.h
  ${TS_FILES}
  )

target_include_directories(QMidi PUBLIC "$(PROJECT_LIBRARY_DIR)" RtMidi)


target_link_libraries(QMidi PRIVATE Qt${QT_VERSION_MAJOR}::Widgets PUBLIC RtMidi)
target_compile_definitions(QMidi PRIVATE QMIDI_LIBRARY)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

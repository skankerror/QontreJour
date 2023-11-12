cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(QontreJour VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

#add_subdirectory(libs/QMidi)
#add_subdirectory(libs/QOSC)
#add_subdirectory(libs/QEnttecPro)
add_subdirectory(libs/QDmxLib)

#add_subdirectory(src)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network SerialPort REQUIRED)
#find_package(ossia REQUIRED)

set(TS_FILES QontreJour_fr_GF.ts)

set(DATA_FILES README.md TODO LICENSE)

set(PROJECT_SOURCES
        src/main.cpp
        src/qontrejour.h
        src/core/dmxvalue.h
        src/core/dmxvalue.cpp
#        src/core/dmxscene.h
#        src/core/dmxscene.cpp
        src/core/dmxmanager.h
        src/core/dmxmanager.cpp
        src/core/dmxengine.h
        src/core/dmxengine.cpp
        src/core/interpreter.h
        src/core/interpreter.cpp
        src/gui/mainwindow.h
        src/gui/mainwindow.cpp
        src/gui/universewidget.h
        src/gui/universewidget.cpp
        src/gui/sequencerwidget.h
        src/gui/sequencerwidget.cpp
        src/gui/valuetablewidget.h
        src/gui/valuetablewidget.cpp
        src/gui/valuesliderswidget.h
        src/gui/valuesliderswidget.cpp
#        src/gui/valueeditwidget.h
#        src/gui/valueeditwidget.cpp
        src/gui/keypadwidget.h
        src/gui/keypadwidget.cpp
        ${TS_FILES}
        ${DATA_FILES}
#        ${BASIC_SOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QontreJour
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/gui/keypadwidget.h src/gui/keypadwidget.cpp
        src/core/interpreter.h src/core/interpreter.cpp
        resources/resources.qrc
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(QontreJour SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(QontreJour
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(QontreJour PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(QontreJour PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(QontreJour PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)
#target_link_libraries(QontreJour PRIVATE ossia)
target_link_libraries(QontreJour PUBLIC #[[QMidi QOSC QEnttecPro]] QDmxLib)

set_target_properties(QontreJour PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QontreJour)
endif()

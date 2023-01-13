cmake_minimum_required(VERSION 3.16)

project(QontreJour VERSION 0.1 LANGUAGES CXX)

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

add_subdirectory(QMidi)
add_subdirectory(QOSC)
add_subdirectory(QEnttecPro)
add_subdirectory(QDmxLib)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network SerialPort REQUIRED)
find_package(ossia REQUIRED)

set(TS_FILES QontreJour_fr_GF.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.h
        mainwindow.cpp
        qontrejour.h
        dimmer.h
        dimmer.cpp
        universe.h
        universe.cpp
        dmxmanagerwidget.h
        dmxmanagerwidget.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QontreJour
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
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
target_link_libraries(QontreJour PRIVATE ossia)
target_link_libraries(QontreJour PUBLIC QMidi QOSC QEnttecPro QDmxLib)

set_target_properties(QontreJour PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QontreJour)
endif()

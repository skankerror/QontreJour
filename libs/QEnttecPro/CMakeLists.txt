cmake_minimum_required(VERSION 3.18)

project(QEnttecPro LANGUAGES CXX VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core SerialPort)

add_library(QEnttecPro STATIC
  qenttecpro.h
  qenttecprodefines.h
  qenttecpro.cpp
  )

target_link_libraries(QEnttecPro PRIVATE Qt6::Core Qt6::SerialPort)

target_compile_definitions(QEnttecPro PRIVATE QENTTECPRO_LIBRARY)

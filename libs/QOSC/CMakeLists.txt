cmake_minimum_required(VERSION 3.18)

project(QOSC LANGUAGES CXX VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(oscpack)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(QOSC STATIC
  qoscargument.h
  qoscsender.h
  qoscsender.cpp
  qoscbundle.h
  qoscbundle.cpp
  qoscmessage.h
  qoscmessage.cpp
  qoscreceiver.h
  qoscreceiver.cpp
  qoscsender.h
  qoscsender.cpp

  oscpack/ip/IpEndpointName.h
  oscpack/ip/NetworkingUtils.h
  oscpack/ip/PacketListener.h
  oscpack/ip/TimerListener.h
  oscpack/ip/UdpSocket.h
  oscpack/osc/MessageMappingOscPacketListener.h
  oscpack/osc/OscException.h
  oscpack/osc/OscHostEndianness.h
  oscpack/osc/OscOutboundPacketStream.h
  oscpack/osc/OscPacketListener.h
  oscpack/osc/OscPrintReceivedElements.h
  oscpack/osc/OscReceivedElements.h
  oscpack/osc/OscTypes.h

  oscpack/ip/IpEndpointName.cpp
  oscpack/osc/OscOutboundPacketStream.cpp
  oscpack/osc/OscPrintReceivedElements.cpp
  oscpack/osc/OscReceivedElements.cpp
  oscpack/osc/OscTypes.cpp

#if(UNIX)
    oscpack/ip/posix/NetworkingUtils.cpp
    oscpack/ip/posix/UdpSocket.cpp
#else()
#    oscpack/ip/win32/NetworkingUtils.cpp
#    oscpack/ip/win32/UdpSocket.cpp
#endif()
)

target_link_libraries(QOSC PRIVATE Qt6::Core)

target_compile_definitions(QOSC PRIVATE QENTTECPRO_LIBRARY)

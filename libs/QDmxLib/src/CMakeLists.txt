set(HEADERS
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxlib_global.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxmanager.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxdriver.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxdriver_p.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxdevice.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxdevice_p.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxgenericnetworkdriver.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxgenericnetworkdriver_p.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxgenericnetworkdevice.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxgenericnetworkdevice_p.h
    ${BUILD_INCLUDE}/qdmxlib/private/qartnetdriver.h
    ${BUILD_INCLUDE}/qdmxlib/private/qartnetdevice.h
    ${BUILD_INCLUDE}/qdmxlib/private/qe131driver.h
    ${BUILD_INCLUDE}/qdmxlib/private/qe131device.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxusbdriver.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxusbdevice.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxusbdevice_p.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxusbbackend_p.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxusbglobal.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxusbinterface.h
    ${BUILD_INCLUDE}/qdmxlib/private/qopendmx.h
    ${BUILD_INCLUDE}/qdmxlib/private/qenttecpro.h
    ${BUILD_INCLUDE}/qdmxlib/private/qeuroliteusbdmxpro.h
    ${BUILD_INCLUDE}/qdmxlib/private/qnanodmx.h
    ${BUILD_INCLUDE}/qdmxlib/private/qvinceusbdmx.h
    ${BUILD_INCLUDE}/qdmxlib/QDmxManager
    ${BUILD_INCLUDE}/qdmxlib/QDmxDriver
    ${BUILD_INCLUDE}/qdmxlib/QDmxDevice
    ${BUILD_INCLUDE}/qdmxlib/QArtnetDriver
    ${BUILD_INCLUDE}/qdmxlib/QArtnetDevice
    ${BUILD_INCLUDE}/qdmxlib/QE131Driver
    ${BUILD_INCLUDE}/qdmxlib/QE131Device
    ${BUILD_INCLUDE}/qdmxlib/QDmxUsbDriver
    ${BUILD_INCLUDE}/qdmxlib/QDmxUsbDevice
)

set(SOURCES
    base/qdmxdevice.cpp
    base/qdmxdriver.cpp
    base/qdmxmanager.cpp

    network/qdmxgenericnetworkdevice.cpp
    network/qdmxgenericnetworkdriver.cpp

    artnet/qartnetdriver.cpp
    artnet/qartnetdevice.cpp

    e131/qe131driver.cpp
    e131/qe131device.cpp

    dmxusb/qdmxusbdriver.cpp
    dmxusb/qdmxusbdevice.cpp
    dmxusb/qdmxusbbackend.cpp
    dmxusb/qdmxusbinterface.cpp
    dmxusb/qopendmx.cpp
    dmxusb/qenttecpro.cpp
    dmxusb/qeuroliteusbdmxpro.cpp
    dmxusb/qnanodmx.cpp
    dmxusb/qvinceusbdmx.cpp
)

if(TARGET Qt::SerialPort)
    list(APPEND SOURCES
        uart/quartdriver.cpp
        uart/quartdevice.cpp
    )

    list(APPEND HEADERS
        ${BUILD_INCLUDE}/qdmxlib/private/quartdriver.h
        ${BUILD_INCLUDE}/qdmxlib/private/quartdevice.h
        ${BUILD_INCLUDE}/qdmxlib/QUartDriver
        ${BUILD_INCLUDE}/qdmxlib/QUartDevice
    )
endif()

if(UNIX AND NOT APPLE)
    list(APPEND SOURCES
        dmx4linux/qdmx4linuxdriver.cpp
        dmx4linux/qdmx4linuxdevice.cpp
    )

    list(APPEND HEADERS
        ${BUILD_INCLUDE}/qdmxlib/private/qdmx4linuxdriver.h
        ${BUILD_INCLUDE}/qdmxlib/private/qdmx4linuxdevice.h
        ${BUILD_INCLUDE}/qdmxlib/QDmx4LinuxDriver
        ${BUILD_INCLUDE}/qdmxlib/QDmx4LinuxDevice
    )
endif()

if(${LibFTDI1_FOUND})
    list(APPEND SOURCES dmxusb/qdmxftdibackend.cpp)
    list(APPEND HEADERS ${BUILD_INCLUDE}/qdmxlib/private/qdmxftdibackend_p.h)
endif()

if(${LibFTD2XX_FOUND})
    list(APPEND SOURCES dmxusb/qdmxftd2xxbackend.cpp)
    list(APPEND HEADERS ${BUILD_INCLUDE}/qdmxlib/private/qdmxftd2xxbackend_p.h)
endif()

if(TARGET Qt::SerialPort)
    list(APPEND SOURCES dmxusb/qdmxserialbackend.cpp)
    list(APPEND HEADERS ${BUILD_INCLUDE}/qdmxlib/private/qdmxserialbackend_p.h)
endif()

add_library(QDmxLib ${HEADERS} ${SOURCES})
target_compile_definitions(QDmxLib PRIVATE QDMXLIB_LIBRARY)
target_compile_definitions(QDmxLib PRIVATE SOFT_VERSION=${PROJECT_VERSION})
target_compile_definitions(QDmxLib PRIVATE GIT_VERSION=${GIT_VERSION})
target_link_libraries(QDmxLib PUBLIC Qt::Core Qt::Network)
target_include_directories(QDmxLib PRIVATE ${${Qt}Core_PRIVATE_INCLUDE_DIRS})

if(${LibFTDI1_FOUND})
    message(STATUS "Build with FTDI support")
    target_compile_definitions(QDmxLib PRIVATE QDMXLIB_HAS_FTDI)
    target_link_directories(QDmxLib PUBLIC ${LIBFTDI_LIBRARY_DIRS})
    target_link_libraries(QDmxLib PUBLIC ${LIBFTDI_LIBRARIES})
    target_include_directories(QDmxLib PUBLIC ${LIBFTDI_INCLUDE_DIRS} "/usr/local/include/libusb-1.0")
endif()

if(${LibFTD2XX_FOUND})
    message(STATUS "Build with FTD2XX support")
    target_compile_definitions(QDmxLib PRIVATE QDMXLIB_HAS_FTD2XX)
    target_link_directories(QDmxLib PUBLIC ${LIBFTD2XX_LIBRARY_DIRS})
    target_link_libraries(QDmxLib PUBLIC ${LIBFTD2XX_LIBRARIES})
    target_include_directories(QDmxLib PUBLIC ${LIBFTD2XX_INCLUDE_DIRS})
endif()

if(TARGET Qt::SerialPort)
    message(STATUS "Build with Qt Serial support")
    target_compile_definitions(QDmxLib PRIVATE QDMXLIB_HAS_QTSERIAL)
    target_link_libraries(QDmxLib PUBLIC Qt::SerialPort)
endif()

target_include_directories(
    QDmxLib
    PUBLIC
        $<BUILD_INTERFACE:${BUILD_INCLUDE}>
        $<INSTALL_INTERFACE:${INCLUDE_DEST}>
    PRIVATE
        ${Qt5Core_PRIVATE_INCLUDE_DIRS}
)

# install
if(${QDMXLIB_INSTALL})
    install(TARGETS QDmxLib EXPORT QDmxLib DESTINATION ${LIB_DEST})
    install(FILES ${BASE_HEADERS} DESTINATION ${INCLUDE_DEST})
endif()

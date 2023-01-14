set(BASE_HEADERS
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxmanager.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxdriver.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxdriver_p.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxdevice.h
    ${BUILD_INCLUDE}/qdmxlib/private/qdmxdevice_p.h
    ${BUILD_INCLUDE}/qdmxlib/QDmxManager
    ${BUILD_INCLUDE}/qdmxlib/QDmxDriver
    ${BUILD_INCLUDE}/qdmxlib/QDmxDevice
)

set(BASE_SOURCES
    qdmxdevice.cpp
    qdmxdriver.cpp
    qdmxmanager.cpp
)

add_library(QDmxLib ${BASE_HEADERS} ${BASE_SOURCES})
target_compile_definitions(QDmxLib PRIVATE QDMXLIB_LIBRARY)
target_compile_definitions(QDmxLib PRIVATE SOFT_VERSION=${PROJECT_VERSION})
target_compile_definitions(QDmxLib PRIVATE GIT_VERSION=${GIT_VERSION})
target_link_libraries(QDmxLib PUBLIC Qt5::Core Qt5::Network)
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

add_subdirectory(core)
add_subdirectory(gui)

set(BASIC_SOURCES
main.cpp
qontrejour.h
${CORE_SOURCES}
${GUI_SOURCES}
)

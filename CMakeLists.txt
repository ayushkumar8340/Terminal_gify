cmake_minimum_required(VERSION 3.5)

project(HeavyTerminal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(VTE REQUIRED vte-2.91)


include_directories(
    include
    ${GTK_INCLUDE_DIRS}
    ${VTE_INCLUDE_DIRS}
)

add_library(heavy_terminal_lib SHARED src/heavy_terminal.cpp)
target_link_libraries(heavy_terminal_lib PUBLIC ${GTK_LIBRARIES} ${VTE_LIBRARIES})

add_executable(heavy_terminal src/main.cpp)
target_link_libraries(heavy_terminal PUBLIC heavy_terminal_lib)

install(TARGETS heavy_terminal_lib LIBRARY DESTINATION /usr/lib)
install(TARGETS heavy_terminal RUNTIME DESTINATION bin)

install(FILES resources/heavy_terminal.png DESTINATION share/icons/hicolor/64x64/apps RENAME heavy-terminal.png)

install(FILES resources/heavy_terminal.desktop DESTINATION share/applications)

install(FILES resources/default_gif.gif DESTINATION ~/.config/heavy_terminal/)

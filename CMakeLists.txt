cmake_minimum_required(VERSION 3.16)

project(QtImageService LANGUAGES CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network HttpServer Gui)

add_executable(QtImageService
  main.cpp
)
target_link_libraries(QtImageService Qt6::Core Qt6::Network Qt6::HttpServer Qt6::Gui)

include(GNUInstallDirs)
install(TARGETS QtImageService
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

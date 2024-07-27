cmake_minimum_required(VERSION 3.5)

project(SharingService)

set(CMAKE_CXX_STANDARD 11)

find_package(Qt6 REQUIRED COMPONENTS DBus)

qt6_wrap_cpp(MOC_SOURCES sharingService.hpp)

add_executable(SharingService main.cpp sharingService.cpp ${MOC_SOURCES})

target_link_libraries(SharingService PRIVATE Qt6::DBus)

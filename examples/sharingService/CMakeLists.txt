cmake_minimum_required(VERSION 3.14)
project(SharingServiceExample)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS DBus)

configure_file(${CMAKE_SOURCE_DIR}/sharingService/include/sharingService.hpp include/sharingService.hpp COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/build/libss.so lib/libss.so COPYONLY)

include_directories(include)

qt6_wrap_cpp(MOC_SOURCES include/sharingService.hpp)

link_directories(lib)

add_executable(SharingServiceExample src/main.cpp ${MOC_SOURCES})

target_link_libraries(SharingServiceExample PRIVATE Qt6::DBus ss)

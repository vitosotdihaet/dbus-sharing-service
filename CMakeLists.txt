cmake_minimum_required(VERSION 3.5)

project(SharingService)

set(CMAKE_CXX_STANDARD 11)

find_package(Qt6 REQUIRED COMPONENTS DBus)

add_compile_options(-Wall -Wextra -Wpedantic)

option(BUILD_SRS "Build the SharingRegisterService" ON)
option(BUILD_LIBSS "Build the SharingService dynamic library" ON)
option(BUILD_EXAMPLES "Build the example" OFF)


# Sharing Register Service
if (BUILD_SRS)
    add_subdirectory(sharingRegisterService)
endif (BUILD_SRS)

# Sharing Service dynamic library
if (BUILD_LIBSS)
    add_subdirectory(sharingService)
endif (BUILD_LIBSS)

# Examples
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)

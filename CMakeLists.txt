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
    set(SHARING_REGISTER_SERVICE ${CMAKE_SOURCE_DIR}/sharingRegisterService)

    set(SHARING_REGISTER_SERVICE_INCLUDE ${SHARING_REGISTER_SERVICE}/include)
    set(SHARING_REGISTER_SERVICE_SOURCES ${SHARING_REGISTER_SERVICE}/src)

    set(SHARING_REGISTER_SERVICE_HEADERS
        ${SHARING_REGISTER_SERVICE_INCLUDE}/sharingRegisterService.hpp
    )

    set(SHARING_REGISTER_SERVICE_SOURCES
        ${SHARING_REGISTER_SERVICE_SOURCES}/main.cpp
        ${SHARING_REGISTER_SERVICE_SOURCES}/sharingRegisterService.cpp
    )

    qt6_wrap_cpp(SHARING_REGISTER_SERVICE_MOC_SOURCES ${SHARING_REGISTER_SERVICE_HEADERS})

    add_executable(srs
        ${SHARING_REGISTER_SERVICE_SOURCES}
        ${SHARING_REGISTER_SERVICE_MOC_SOURCES}
    )

    target_include_directories(srs PRIVATE ${SHARING_REGISTER_SERVICE_INCLUDE})
    target_link_libraries(srs PRIVATE Qt6::DBus)
endif (BUILD_SRS)


# Sharing Service dynamic library
if (BUILD_LIBSS)
    set(SHARING_SERVICE ${CMAKE_SOURCE_DIR}/sharingService)

    set(SHARING_SERVICE_INCLUDE ${SHARING_SERVICE}/include)
    set(SHARING_SERVICE_SOURCES ${SHARING_SERVICE}/src)

    set(SHARING_SERVICE_HEADERS
        ${SHARING_SERVICE_INCLUDE}/sharingService.hpp
    )

    set(SHARING_SERVICE_SOURCES
        ${SHARING_SERVICE_SOURCES}/sharingService.cpp
    )

    add_library(ss SHARED
        ${SHARING_SERVICE_HEADERS}
        ${SHARING_SERVICE_SOURCES}
    )

    target_include_directories(ss PRIVATE ${SHARING_SERVICE_INCLUDE})

    target_link_libraries(ss PRIVATE Qt6::DBus)
    # Examples
    if (BUILD_EXAMPLES)
        add_subdirectory(${CMAKE_SOURCE_DIR}/examples)
    endif (BUILD_EXAMPLES)
endif (BUILD_LIBSS)


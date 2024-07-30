project(SharingServiceExample)

add_custom_target(
    COMMAND configure_file(${CMAKE_SOURCE_DIR}/build/libss.so lib/libss.so COPYONLY)
    MAIN_DEPENDENCY ss
    DEPENDS ${CMAKE_SOURCE_DIR}/build/libss.so
)

configure_file(${CMAKE_SOURCE_DIR}/sharingService/include/sharingService.hpp include/sharingService.hpp COPYONLY)

include_directories(include)

qt6_wrap_cpp(MOC_SOURCES include/sharingService.hpp)

link_directories(lib)

add_executable(SharingServiceExample src/main.cpp ${MOC_SOURCES})

target_link_libraries(SharingServiceExample PRIVATE Qt6::Core Qt6::DBus ss)

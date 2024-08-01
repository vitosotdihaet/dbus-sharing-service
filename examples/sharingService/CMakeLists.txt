project(SharingServiceExample)



include_directories(include)

qt6_wrap_cpp(MOC_SOURCES include/sharingService.hpp)

link_directories(lib)

add_executable(SharingServiceExample src/main.cpp ${MOC_SOURCES})

target_link_libraries(SharingServiceExample PRIVATE Qt6::Core Qt6::DBus ss)

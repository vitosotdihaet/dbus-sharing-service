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

target_link_libraries(ss PRIVATE Qt6::Core Qt6::DBus)


add_custom_command(
    TARGET ss
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ss>
        ${CMAKE_SOURCE_DIR}/examples/sharingService/lib/libss.so
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHARING_SERVICE_INCLUDE}/sharingService.hpp
        ${CMAKE_SOURCE_DIR}/examples/sharingService/include/sharingService.hpp
)

add_custom_target(copy_ss_files ALL
    DEPENDS ss
    COMMENT "Copying files produced by 'ss' target"
)

add_dependencies(copy_ss_files ss)
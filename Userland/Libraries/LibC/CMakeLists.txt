set(LIBC_SOURCES
    sys/syscall.c
    stdio.c
    stdlib.c
    string.c
)

set(SOURCES ${LIBC_SOURCES})

add_library(LibCStaticWithoutDeps STATIC ${SOURCES})

add_custom_target(LibCStatic
    COMMAND ${CMAKE_AR} -x $<TARGET_FILE:LibCStaticWithoutDeps>
    COMMAND ${CMAKE_AR} -qcs ${CMAKE_CURRENT_BINARY_DIR}/libc.a *.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS LibCStaticWithoutDeps
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libc.a DESTINATION ${CMAKE_INSTALL_PREFIX}/usr/lib)
file(GLOB TEMP_OBJ_FILES ${CMAKE_CURRENT_BINARY_DIR}/*.o)
set_property(
    TARGET LibCStatic
    APPEND
    PROPERTY ADDITIONAL_CLEAN_FILES ${TEMP_OBJ_FILES}
)

theos_libc(LibC c)

add_dependencies(LibC LibCStatic)
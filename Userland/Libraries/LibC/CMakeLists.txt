set(LIBC_SOURCES
    sys/syscall.c
    config/keyboard.c
    stdio.c
    stdlib.c
    string.c
    unistd.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTHEOS_LIBC_BUILD")

add_library(crt0 STATIC crt0.S)
add_custom_command(
    TARGET crt0
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:crt0> ${CMAKE_INSTALL_PREFIX}/usr/lib/crt0.o
)

set_source_files_properties(ssp.c PROPERTIES COMPILE_FLAGS
    "-fno-stack-protector")
add_library(ssp STATIC ssp.c)
add_custom_command(
    TARGET ssp
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:ssp> ${CMAKE_INSTALL_PREFIX}/usr/lib/ssp.o   
)

set(SOURCES ${LIBC_SOURCES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
add_library(LibCStaticWithoutDeps STATIC ${SOURCES})
target_link_libraries(LibCStaticWithoutDeps crt0 ssp)

add_custom_target(LibCStatic
    COMMAND ${CMAKE_AR} -x $<TARGET_FILE:LibCStaticWithoutDeps>
    COMMAND ${CMAKE_AR} -x $<TARGET_FILE:ssp>
    COMMAND ${CMAKE_AR} -qcs ${CMAKE_CURRENT_BINARY_DIR}/libc.a *.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS LibCStaticWithoutDeps ssp
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libc.a DESTINATION usr/lib)
file(GLOB TEMP_OBJ_FILES ${CMAKE_CURRENT_BINARY_DIR}/*.o)
set_property(
    TARGET LibCStatic
    APPEND
    PROPERTY ADDITIONAL_CLEAN_FILES ${TEMP_OBJ_FILES}
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -static-libgcc")
theos_libc(LibC c)
target_link_libraries(LibC crt0 ssp)

add_dependencies(LibC LibCStatic)
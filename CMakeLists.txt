cmake_minimum_required(VERSION 4.2)

project(nsargs)

option(UNICODE "Build with Unicode support" ON)

file(GLOB SRC 
    nsis/api.h
    nsis/nsis_tchar.h
    nsis/pluginapi.h
    nsis/pluginapi.c

    src/args.h
    src/args.c
    src/util.h
    src/util.c
    src/nsargs.c
)

message(STATUS "${CMAKE_CURRENT_LIST_DIR}")

add_library(${PROJECT_NAME} SHARED ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE user32)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set(OUTPUT_PATH "")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    string(APPEND OUTPUT_PATH "x64")
else()
    string(APPEND OUTPUT_PATH "x86")
endif()

if (UNICODE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
    string(APPEND OUTPUT_PATH "-unicode")
else()
    string(APPEND OUTPUT_PATH "-ansi")
endif()

foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/${OUTPUT_PATH}
    )
endforeach()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CTest)

    find_package(cmocka CONFIG REQUIRED)

    set(TEST_SRC
        test/main.c
 
        nsis/pluginapi.c
 
        src/nsargs.c
        src/args.c
        src/util.c
    )
 
    add_executable(nsargs_test ${TEST_SRC})
 
    target_link_libraries(nsargs_test PRIVATE user32 cmocka::cmocka)
    target_compile_definitions(nsargs_test PRIVATE -DTESTING)
 
    target_include_directories(nsargs_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/nsis
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
 
    if (UNICODE)
        target_compile_definitions(nsargs_test PRIVATE UNICODE _UNICODE)
    endif()

    add_test(NAME nsargs_test COMMAND nsargs_test)
endif()
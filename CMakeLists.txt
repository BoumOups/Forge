cmake_minimum_required(VERSION 4.1.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("Forge" CXX)

set("WASMTIME_C_DIR" ${CMAKE_CURRENT_SOURCE_DIR}/vendor/wasmtime/wasmtime-c-api)
set("WASMTIME_CPP_DIR" ${CMAKE_CURRENT_SOURCE_DIR}/vendor/wasmtime/wasmtime-cpp)
if (APPLE)
    find_library("WASMTIME_LIB" wasmtime HINTS "${WASMTIME_C_DIR}/lib/apple")
endif()
if(LINUX)
    find_library("WASMTIME_LIB" wasmtime HINTS "${WASMTIME_C_DIR}/lib/linux")
endif()
if(WIN32)
    if(MINGW)
        set(WASMTIME_LIB "${WASMTIME_C_DIR}/lib/windows/wasmtime.dll")
    else()
        find_library("WASMTIME_LIB" wasmtime HINTS "${WASMTIME_C_DIR}/lib/windows")
        if (NOT WASMTIME_LIB AND EXISTS "${WASMTIME_C_DIR}/lib/windows/wasmtime.dll")
            set(WASMTIME_LIB "${WASMTIME_C_DIR}/lib/windows/wasmtime.dll")
        endif()
    endif()
endif()

set("SOURCES"
        "src/main.cpp"
        "src/sandbox/loader.cpp"
        "src/cli/interface.cpp"
        "src/core/core.cpp"
        "src/core/paths.cpp"
        include/forge_wasm.hpp
)

add_executable("forge" ${SOURCES})

target_include_directories("forge" PRIVATE
        "include"
        "src"
        "vendor"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/json"
        "${WASMTIME_C_DIR}/include"
        "${WASMTIME_CPP_DIR}/include"
)
if(APPLE)
    target_link_libraries("forge" PRIVATE ${WASMTIME_LIB})
endif()
if(LINUX)
    target_link_libraries("forge" PRIVATE ${WASMTIME_LIB})
endif()
if(WIN32)
    target_link_libraries("forge" PRIVATE ${WASMTIME_LIB} "-lstdc++exp")
endif()


if(APPLE)
    add_custom_command(TARGET forge POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WASMTIME_C_DIR}/lib/apple/libwasmtime.dylib"
        "$<TARGET_FILE_DIR:forge>/libwasmtime.dylib"
    )
    set_target_properties(forge PROPERTIES
        INSTALL_RPATH "@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()
if (LINUX)
    add_custom_command(TARGET forge POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WASMTIME_C_DIR}/lib/linux/libwasmtime.so"
        "$<TARGET_FILE_DIR:forge>/libwasmtime.so"
    )
    set_target_properties(forge PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()
if(WIN32)
    add_custom_command(TARGET forge POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WASMTIME_C_DIR}/lib/windows/wasmtime.dll"
        "$<TARGET_FILE_DIR:forge>/wasmtime.dll"
    )
    set_target_properties( forge PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

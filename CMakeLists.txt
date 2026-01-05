cmake_minimum_required(VERSION 4.1.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("Forge" CXX)

find_package("OpenSSL" REQUIRED)

set("WASMTIME_C_DIR" ${CMAKE_CURRENT_SOURCE_DIR}/vendor/wasmtime/wasmtime-c-api)
set("WASMTIME_CPP_DIR" ${CMAKE_CURRENT_SOURCE_DIR}/vendor/wasmtime/wasmtime-cpp)
if (APPLE)
    find_library("WASMTIME_LIB" wasmtime HINTS "${WASMTIME_C_DIR}/lib/apple")
endif()
if(LINUX)
    find_library("WASMTIME_LIB" wasmtime HINTS "${WASMTIME_C_DIR}/lib/linux")
endif()
if(WIN32)
    find_library("WASMTIME_LIB" wasmtime HINTS "${WASMTIME_C_DIR}/lib/windows")
endif()

set("SOURCES"
        "src/main.cpp"
        "src/sandbox/loader.cpp"
        "src/cli/interface.cpp"
        "src/core/core.cpp"
)

add_executable("forge" ${SOURCES})

target_include_directories("forge" PRIVATE
        "include"
        "src"
        "vendor"
        "${WASMTIME_C_DIR}/include"
        "${WASMTIME_CPP_DIR}/include"
)
target_link_libraries("forge" PRIVATE OpenSSL::Crypto ${WASMTIME_LIB})


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

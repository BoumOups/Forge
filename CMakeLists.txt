cmake_minimum_required(VERSION 4.1.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("Forge" CXX)

find_package("OpenSSL" REQUIRED)

set("WASMTIME_C_DIR" ${CMAKE_CURRENT_SOURCE_DIR}/vendor/wasmtime-c-api)
set("WASMTIME_CPP_DIR" ${CMAKE_CURRENT_SOURCE_DIR}/vendor/wasmtime-cpp)
find_library("WASMTIME_LIB" wasmtime HINTS "${WASMTIME_C_DIR}/lib")

add_compile_options("-rdynamic")

set("SOURCES"
    "src/main.cpp"
    "src/loader.cpp"
)

add_executable("forge" ${SOURCES})

target_include_directories("forge" PRIVATE
    "include"
    "vendor"
    "${WASMTIME_C_DIR}/include"
    "${WASMTIME_CPP_DIR}/include"
)
target_link_libraries("forge" PRIVATE OpenSSL::Crypto ${WASMTIME_LIB})

add_custom_command(TARGET forge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${WASMTIME_C_DIR}/lib/libwasmtime.dylib"
    "$<TARGET_FILE_DIR:forge>/libwasmtime.dylib"
)

if(APPLE)
    set_target_properties(forge PROPERTIES
        INSTALL_RPATH "@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

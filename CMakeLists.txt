cmake_minimum_required(VERSION 4.2.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("Forge" CXX)

find_package("OpenSSL" REQUIRED)

add_compile_options("-rdynamic")

set("SOURCES"
    "src/main.cpp"
    "src/loader.cpp"
)
add_executable("forge" ${SOURCES})

target_include_directories("forge" PRIVATE "include" "vendor")
target_link_libraries("forge" PRIVATE OpenSSL::Crypto)

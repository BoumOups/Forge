cmake_minimum_required(VERSION 4.2.1)

project("Forge")

set("SOURCES"
    "src/main.cpp"
)
add_executable("forge" ${SOURCES})

target_include_directories("forge" PRIVATE "include")

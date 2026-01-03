project('forge', 'cpp', default_options: ['cpp_std=c++23'])
add_project_link_arguments('-lstdc++fs', language: 'cpp')

includes = include_directories('include')

executable('forge', 'src/main.cpp', include_directories: includes)

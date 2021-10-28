# 3.15 for GLOB_RECURSE
cmake_minimum_required(VERSION 3.15)

project (CSH)
file(GLOB_RECURSE CSH_SOURCE src/csh/*.cpp src/csh/*.h)

add_library(CSH ${CSH_SOURCE})
target_include_directories(CSH PUBLIC src)
# Make sure you have the Microsoft Windows Toolkit Installed
target_link_libraries(CSH PUBLIC d3d11.lib d3dcompiler.lib)

project(Sandbox)
file(GLOB_RECURSE SANDBOX_SOURCE Sandbox/*.cpp Sandbox/*.h)

add_executable(Sandbox ${SANDBOX_SOURCE})
target_link_libraries(Sandbox PRIVATE CSH)
target_include_directories(Sandbox PRIVATE src)
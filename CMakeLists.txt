# CMakeLists files in this project can
# refer to the root source directory of the project as ${NXPRDLIBLINUX_SOURCE_DIR} and
# to the root binary directory of the project as ${NXPRDLIBLINUX_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (NXPRDLIBLINUX)

add_subdirectory (linux)
add_subdirectory (examples)

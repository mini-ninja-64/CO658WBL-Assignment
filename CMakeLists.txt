cmake_minimum_required (VERSION 3.15)

include(cmake/helpers.cmake)
setup_google_test()

set(CMAKE_CXX_STANDARD 20)
project ("CO658WBL Assignment")

add_subdirectory("app")
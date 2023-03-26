cmake_minimum_required(VERSION 3.18)
project(cxxstreams LANGUAGES C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;")
include(LibraryProject)
lib_define_targets()
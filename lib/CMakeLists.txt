# in dir lib/
cmake_minimum_required(VERSION 2.8)

add_library(heat heat.c)
install(TARGETS heat DESTINATION lib)
# in dir lib/
cmake_minimum_required(VERSION 3.3)

add_library(heat heat.c)
install(TARGETS heat DESTINATION lib)
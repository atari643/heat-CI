# CTest script for Build without MPI

set(CMAKE_HANDS_ON_DIR "$ENV{CMAKE_HANDS_ON}")
if (DEFINED CMAKE_HANDS_ON_DIR)
  set(CTEST_SOURCE_DIRECTORY "${CMAKE_HANDS_ON_DIR}/exe6_ctest")
  set(CTEST_BINARY_DIRECTORY "${CMAKE_HANDS_ON_DIR}/exe6_ctest/build_mpi")
else()
  message(FATAL_ERROR "CMAKE_HANDS_ON env. variable is not set, please indicate where are your sources.")
endif()

# which ctest command to use for running the dashboard
set(CTEST_COMMAND "ctest -D Nightly")

set(WITH_MEMCHECK TRUE)
set(WITH_COVERAGE FALSE)

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND "cmake")

# should ctest wipe the binary tree before running
set(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

# set any extra environment variables to use during the execution of the script here:
set(CTEST_ENVIRONMENT)

# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
set(CTEST_INITIAL_CACHE "
CMAKE_BUILD_TYPE:STRING=Debug
BUILD_SHARED_LIBS:BOOL=ON
HEAT_USE_MPI:BOOL=ON
")

#set(CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} -D")
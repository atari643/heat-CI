# CTest script for Build without MPI

set(HEAT_DIR "$ENV{HOME}/git/heat")
message(STATUS "HEAT_DIR ${HEAT_DIR}")

set(CTEST_SOURCE_DIRECTORY "/home/florent/git/heat")
set(CTEST_BINARY_DIRECTORY "/home/florent/git/heat/build")
message(STATUS "CTEST_BINARY_DIRECTORY ${CTEST_BINARY_DIRECTORY}")

set(_auth_token "fc07fd1c3a678265bc928ab05a361603")
ctest_submit(HTTPHEADER “Authorization: Bearer ${_auth_token}“)

# which ctest command to use for running the dashboard
set(CTEST_COMMAND "ctest -D Experimental")

#set(WITH_MEMCHECK TRUE)
#set(WITH_COVERAGE FALSE)

# what cmake command to use for configuring this dashboard
#SET (CTEST_CMAKE_COMMAND "cmake")

# should ctest wipe the binary tree before running
#set(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

# set any extra environment variables to use during the execution of the script here:
#set(CTEST_ENVIRONMENT)

# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
set(CTEST_INITIAL_CACHE "
CMAKE_BUILD_TYPE:STRING=Release
BUILD_SHARED_LIBS:BOOL=ON
HEAT_USE_MPI:BOOL=OFF
")

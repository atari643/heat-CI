cmake_minimum_required (VERSION 3.1)
project (heat C)
include(CMakeDependentOption)

set(HEAT_VERSION_MAJOR "1")
set(HEAT_VERSION_MINOR "0")
set(HEAT_VERSION_PATCH "0")
set(HEAT_VERSION "${HEAT_VERSION_MAJOR}.${HEAT_VERSION_MINOR}.${HEAT_VERSION_PATCH}")

option(HEAT_USE_MPI "Build MPI executable" OFF)
option(HEAT_DOC "Build the doxygen documentation" OFF)

# Set the RPATH config
# --------------------
# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
list(APPEND CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# libheat
include_directories(include)
add_subdirectory(lib)

# heat_seq exe
set(HEAT_SOURCES heat_seq.c mat_utils.c)
add_executable(heat_seq ${HEAT_SOURCES} mat_utils.h)
target_link_libraries(heat_seq heat m)
# install heat_seq
install(TARGETS heat_seq DESTINATION bin)

# heat_par exe - depends on MPI
if (HEAT_USE_MPI)

    find_package(MPI)
    if(MPI_FOUND)
        add_executable(heat_par heat_par.c mat_utils.c mat_utils.h)
        target_link_libraries(heat_par MPI::MPI_C heat m)
        install(TARGETS heat_par DESTINATION bin)
    else(MPI_FOUND)
        message(WARNING ”MPI not found” )
    endif(MPI_FOUND)

endif(HEAT_USE_MPI)

# uninstall target
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_modules/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_modules/cmake_uninstall.cmake")

# static analysis during build
find_program(CPPCHECK "cppcheck")
if (CPPCHECK)
  set(CMAKE_C_CPPCHECK "${CPPCHECK}"
    "--language=c"
    "--platform=unix64"
    "--enable=all"
    "--force"
    "--inline-suppr"
    )
endif()
find_program(CLANGTIDY "clang-tidy")
if (CLANGTIDY)
  set(CMAKE_C_CLANG_TIDY "${CLANGTIDY}")
endif()

# tests
find_program( MEMORYCHECK_COMMAND valgrind )
set( MEMORYCHECK_COMMAND_OPTIONS "--xml=yes --xml-file=heat-valgrind.xml --memcheck:leak-check=full --show-reachable=yes" CACHE STRING "" FORCE )
set( MEMORYCHECK_SUPPRESSIONS_FILE "/usr/share/openmpi/openmpi-valgrind.supp" CACHE FILEPATH "" FORCE)
include(CTest)
if(BUILD_TESTING)
  # ... CMake code to create tests ...
  add_test(heat_seq_usage ./heat_seq)
  set_tests_properties(heat_seq_usage PROPERTIES PASS_REGULAR_EXPRESSION "Usage*")
  add_test(heat_seq_err_10 ./heat_seq 10 10 1 1 1)
  #set_tests_properties(heat_seq_err_10 PROPERTIES PASS_REGULAR_EXPRESSION "1.732*")
  if(HEAT_USE_MPI AND MPI_FOUND)
    add_test(heat_par_4 ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ./heat_par 10 10 200 2 2 0)
  endif(HEAT_USE_MPI AND MPI_FOUND)
endif()

# documentation
if (HEAT_DOC)
  add_subdirectory(doc)
endif()

# package
set(CPACK_GENERATOR "DEB")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "heat")
set(CPACK_PACKAGE_VENDOR "inria-bso-sed")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "inria-bso-sed")
set(CPACK_PACKAGE_VERSION "${HEAT_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "heat-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES "/build/;/formation/;/.git/;.gitignore;/tools/")
include(CPack)

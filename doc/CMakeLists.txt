cmake_minimum_required(VERSION 3.3)

# Looking for doxygen
# -------------------
find_package(Doxygen)

# Generate documentation
# ----------------------
if(DOXYGEN_EXECUTABLE)
  message(STATUS "Looking for doxygen - found")
  configure_file("Doxyfile.in" "Doxyfile" @ONLY)
  # User's documentation
  # --------------------
  add_custom_command(OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/html
                     COMMAND ${DOXYGEN_EXECUTABLE}
                     ARGS    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    )
  add_custom_target(doc ALL
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html
                    )
  # Installation for html version
  # -----------------------------
  install(DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/html
          DESTINATION share/doxygen/)
else()
    message(FATAL_ERROR "Looking for doxygen - not found")
endif()

###
### END CMakeLists.txt
###

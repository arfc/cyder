# ------------------- Add all Concrete Agents ----------------------------

EXECUTE_PROCESS(COMMAND git describe --tags OUTPUT_VARIABLE cyder_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE(cyder_version.h.in "${CMAKE_CURRENT_SOURCE_DIR}/cyder_version.h" @ONLY)

SET(CYCLUS_CUSTOM_HEADERS "cyder_version.h")

USE_CYCLUS("cyder" "conditioning")
INSTALL_CYCLUS_MODULE("cyder" "" "NONE")

SET(TestSource ${cyder_TEST_CC} PARENT_SCOPE)

# install header files
FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

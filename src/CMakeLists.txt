# ------------------- Add all Concrete Agents ----------------------------

EXECUTE_PROCESS(COMMAND git describe --tags OUTPUT_VARIABLE recycle_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE(recycle_version.h.in "${CMAKE_CURRENT_SOURCE_DIR}/recycle_version.h" @ONLY)

SET(CYCLUS_CUSTOM_HEADERS "recycle_version.h")

USE_CYCLUS("recycle" "corrm")

INSTALL_CYCLUS_MODULE("recycle" "" "NONE")

SET(TestSource ${recycle_TEST_CC} PARENT_SCOPE)

# install header files
FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

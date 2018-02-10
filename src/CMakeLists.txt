# ------------------- Add all Concrete Agents ----------------------------

EXECUTE_PROCESS(COMMAND git describe --tags OUTPUT_VARIABLE recycle_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE(recycle_version.h.in "${CMAKE_CURRENT_SOURCE_DIR}/recycle_version.h" @ONLY)

SET(CYCLUS_CUSTOM_HEADERS "recycle_version.h")

USE_CYCLUS("recycle" "reactor")

USE_CYCLUS("recycle" "fuel_fab")

USE_CYCLUS("recycle" "mixer")

USE_CYCLUS("recycle" "enrichment")

USE_CYCLUS("recycle" "separations")

USE_CYCLUS("recycle" "sink")

USE_CYCLUS("recycle" "source")

USE_CYCLUS("recycle" "deploy_inst")

USE_CYCLUS("recycle" "manager_inst")

USE_CYCLUS("recycle" "growth_region")

USE_CYCLUS("recycle" "storage")

INSTALL_CYCLUS_MODULE("recycle" "" "NONE")

SET(TestSource ${recycle_TEST_CC} PARENT_SCOPE)

# install header files
FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

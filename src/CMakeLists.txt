### DO NOT DELETE THIS COMMENT: INSERT_ARCHETYPES_HERE ###

USE_CYCLUS("cyder" "Component")
USE_CYCLUS("cyder" "Cyder")
USE_CYCLUS("cyder" "DegRateNuclide")
USE_CYCLUS("cyder" "Geometry")
USE_CYCLUS("cyder" "LumpedNuclide")
USE_CYCLUS("cyder" "MatTools")
USE_CYCLUS("cyder" "MixedCellNuclide")
USE_CYCLUS("cyder" "NuclideModelFactory")
USE_CYCLUS("cyder" "OneDimPPMNuclide")
USE_CYCLUS("cyder" "STCThermal")
USE_CYCLUS("cyder" "SolLim")
USE_CYCLUS("cyder" "STCThermal")
USE_CYCLUS("cyder" "StubNuclide")
USE_CYCLUS("cyder" "StubThermal")
USE_CYCLUS("cyder" "ThermalModel")
USE_CYCLUS("cyder" "ThermalModelFactory")

INSTALL_CYCLUS_MODULE("cyder" "" "NONE")

# install header files
FILE(GLOB h_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

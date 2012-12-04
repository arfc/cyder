# Find tests generated in the binary directory
message("GENERATED TESTS DIR ${CMAKE_CURRENT_BINARY_DIR}")
message("GENERATED TESTS ${GENERATED_FILTER_LIST}")

# To add a new file, just add it to this list.  Any GoogleTests inside will be automatically
# added to ctest.
set ( CYDER_TEST_CORE 
  ${CMAKE_CURRENT_SOURCE_DIR}/ComponentTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DegRateNuclideTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GenericRepositoryTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GeometryTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/LumpedNuclideTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MixedCellNuclideTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MaterialDBTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/OneDimPPMNuclideTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TwoDimPPMNuclideTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StubNuclideTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MatToolsTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/NuclideModelTests.cpp
  ${CYCLUS_CORE_INCLUDE_DIR}/FacilityModelTests.cpp
  ${CYCLUS_CORE_INCLUDE_DIR}/ModelTests.cpp
  PARENT_SCOPE)

# Add all the tests by parsing the source code
# This macro searches for GoogleTest macros and adds them as tests automatically
macro(ADD_GOOGLE_TESTS executable)
  # Add the generated tests
  set ( AllTestsHits ${GENERATED_TEST_LIST} )

  foreach ( source ${ARGN} )
    file(READ "${source}" contents)

    # Find all test lists
    string(REGEX MATCHALL "TEST_?[FP]?\\(([A-Za-z_0-9 ,_]+)\\)" AllTests ${contents})

    # Convert the C++ code into a short test name
    foreach(hit ${AllTests})
      string(REGEX REPLACE ".*\\(( *[A-Za-z_0-9]+)[, ]*([A-Za-z_0-9]+) *\\).*" "\\1.\\2" test_name ${hit})
      set ( AllTestsHits ${AllTestsHits} ${test_name} )
    endforeach()
  endforeach()

  list ( SORT AllTestsHits )
  # Add all the remaining tests to CTest's test list
  foreach(hit ${AllTestsHits})
    add_test(${hit} ${executable} --gtest_filter=${hit})
  endforeach()
endmacro()




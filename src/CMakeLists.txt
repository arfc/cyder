
# This project name is cycero
PROJECT(cycero)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Direct any out-of-source builds to this directory
SET( CYCERO_SOURCE_DIR ${CMAKE_SOURCE_DIR} )

# Direct any binary installation paths to this directory
SET( CYCERO_BINARY_DIR ${CMAKE_BINARY_DIR} )

# This makes all the libraries build as SHARED
SET(BUILD_SHARED_LIBS true)

# Setup build locations.
IF(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CYCERO_BINARY_DIR}/bin)
endif()
IF(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CYCERO_BINARY_DIR}/lib)
endif()
IF(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CYCERO_BINARY_DIR}/lib)
ENDIF()

SET(CYCERO_EXECUTABLE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/cycero/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/cycero/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/cycero/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

# Tell CMake where the modules are
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_DIR}/share/cmake-2.8/Modules")
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CYCERO_SOURCE_DIR}/CMake )

# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
FIND_PACKAGE( LibXml2 REQUIRED )
SET(CYCERO_INCLUDE_DIR ${CYCERO_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
ADD_DEFINITIONS (${LIBXML2_DEFINITIONS})
SET(LIBS ${LIBS} ${LIBXML2_LIBRARIES})

# Find Sqlite3
FIND_PACKAGE( Sqlite3 REQUIRED )
SET(CYCERO_INCLUDE_DIR ${CYCERO_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR})
SET(LIBS ${LIBS} ${SQLITE3_LIBRARIES})

# Find cycluscore
FIND_PACKAGE( cycluscore REQUIRED )
SET(CYCERO_INCLUDE_DIR ${CYCERO_INCLUDE_DIR} ${CYCLUS_CORE_INCLUDE_DIR})
SET(LIBS ${LIBS} ${CYCLUS_CORE_LIBRARIES})

# Include the boost header files and the program_options library
SET(Boost_USE_STATIC_LIBS       OFF)
SET(Boost_USE_STATIC_RUNTIME    OFF)
FIND_PACKAGE( Boost COMPONENTS program_options filesystem system REQUIRED)
SET(CYCERO_INCLUDE_DIR ${CYCERO_INCLUDE_DIR} ${BOOST_INCLUDE_DIR})
SET(LIBS ${LIBS} ${Boost_PROGRAM_OPTIONS_LIBRARY})
SET(LIBS ${LIBS} ${Boost_SYSTEM_LIBRARY})
SET(LIBS ${LIBS} ${Boost_FILESYSTEM_LIBRARY})

# include the model directories
SET(CYCERO_INCLUDE_DIR ${CYCERO_INCLUDE_DIR} Testing )
SET(CYCERO_INCLUDE_DIR ${CYCERO_INCLUDE_DIR} ${CYCERO_SOURCE_DIR})

# include all the directories we just found
INCLUDE_DIRECTORIES( ${CYCERO_INCLUDE_DIR} )

# This is the directory that holds the doxygen doxyfile template (doxy.conf.in)
SET( DOC_INPUT_DIR ${CYCERO_SOURCE_DIR}/doc)

# If doxygen exists, use the doc/CMakeLists.txt file for further instructions. 
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
  ADD_SUBDIRECTORY(doc)
  SET( DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc )
ELSE (DOXYGEN_FOUND)
  MESSAGE(STATUS "WARNING: Doxygen not found - doc won't be created")
ENDIF (DOXYGEN_FOUND)

# ------------------------- Add the Models -----------------------------------
SET(MODEL_PATH "/Models/Facility/GenericRepository")

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${MODEL_PATH})

SET(GenericRepository_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/GenericRepository.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Component.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DegRateNuclide.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/LumpedNuclide.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MixedCellNuclide.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/OneDimPPMNuclide.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StubNuclide.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StubThermal.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TwoDimPPMNuclide.cpp
  )

ADD_LIBRARY( GenericRepository ${GenericRepository_SRC})

SET(CYCERO_LIBRARIES ${CYCERO_LIBRARIES} GenericRepository )

TARGET_LINK_LIBRARIES( GenericRepository dl ${CYCERO_LIBRARIES} ${LIBS})

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/GenericRepository.rng
  ${PROJECT_BINARY_DIR}/lib${MODEL_PATH}/GenericRepository.rng
  COPYONLY
  )

install(TARGETS GenericRepository
  LIBRARY DESTINATION cyclus/lib${MODEL_PATH}
  COMPONENT ${MODEL_PATH}
  )

SET(RNG_INCLUDES ${RNG_INCLUDES}
  "<include href='../lib${MODEL_PATH}/GenericRepository.rng'/>"
  )

install(FILES "GenericRepository.rng"
  DESTINATION cyclus/lib${MODEL_PATH}
  COMPONENT "${MODEL_PATH}.rng"
  )

SET(FacilityTestSource ${FacilityTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/GenericRepositoryTests.cpp 
  ${GenericRepository_SRC}
  )

SET(Facility_REFS ${Facility_REFS}
  "<ref name='GenericRepository'/>"
  )

CONFIGURE_FILE(${CYCLUS_CORE_SHARE_DIR}/cyclus.rng.in
  ${PROJECT_BINARY_DIR}/share/cyclus.rng @ONLY
  )

INSTALL(FILES 
  ${PROJECT_BINARY_DIR}/share/cyclus.rng
  DESTINATION ${CYCLUS_CORE_SHARE_DIR}
  COMPONENT data
  )

# ------------------------- Google Test -----------------------------------

# Be sure to clear these each time
SET( GENERATED_TEST_LIST "" CACHE INTERNAL "" )
SET( GENERATED_FILTER_LIST "" CACHE INTERNAL "" )
SET( GENERATED_TEST_SOURCE_LIST "" CACHE INTERNAL "" )

# The gtest include file directories.
SET(GTEST_INCLUDE_DIRS "${CYCERO_INCLUDE_DIR}")

# Alerts CMake to any tests we need to worry about
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

# Create an executable for all the gtests
# Options for testing
OPTION( USE_TESTING "Build testing" ON )
IF( USE_TESTING )
  enable_testing()
  INCLUDE(CTest)
ENDIF()

ADD_SUBDIRECTORY(Testing)
SET(TestSource 
  ${TestSource} 
  ${CYCERO_TEST_CORE}
  )

FIND_PACKAGE(Threads)
IF(CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
  SET(cxx_base_flags "${cxx_base_flags} -DGTEST_HAS_PTHREAD=1")
ENDIF()

# Build CyceroUnitTestDriver
ADD_EXECUTABLE( CyceroUnitTestDriver
  Testing/CyceroUnitTestDriver.cpp ${TestSource}
  ${GENERATED_TEST_SOURCE}
)
TARGET_LINK_LIBRARIES( CyceroUnitTestDriver dl ${CYCERO_LIBRARIES} 
  dl ${LIBS} ${CYCLUS_GTEST_LIBRARIES})

# Here we add all the tests
add_google_tests( ${EXECUTABLE_OUTPUT_PATH}/bin/CyceroUnitTestDriver 
  ${TestSource}) 

INSTALL(TARGETS CyceroUnitTestDriver
  RUNTIME DESTINATION cycero/bin
  COMPONENT testing
  )

FILE(GLOB cyclus_shared "${CYCLUS_CORE_SHARE_DIR}/*")
INSTALL(FILES ${cyclus_shared} 
  DESTINATION cycero/share
  COMPONENT data
  )

# Here we set some components for installation with cpack
SET(CPACK_COMPONENTS_ALL cycero)

# Here's where we package it with CPack
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cycero : The Cyclus Environment and Repository Object.")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "CNERG, UW-Madison") #required
SET(CPACK_PACKAGE_VENDOR "CNERG, UW-Madison")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CMake/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/CMake/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
#SET(CPACK_PACKAGE_VERSION_PATCH "2") # should use commit number here  ?
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "cycero${CPACK_VERSION_MAJOR}.${CPACK_VERSION_MINOR}")
SET(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
 
CONFIGURE_FILE("${CYCERO_SOURCE_DIR}/CMake/CyceroCPackOptions.cmake.in"
  "${CYCERO_BINARY_DIR}/CMake/CyceroCPackOptions.cmake" @ONLY)
SET(CPACK_PROJECT_CONFIG_FILE "${CYCERO_BINARY_DIR}/CMake/CyceroCPackOptions.cmake")
SET(CPACK_PACKAGE_EXECUTABLES "cycero" "CyceroUnitTestDriver")


INCLUDE(CPack)



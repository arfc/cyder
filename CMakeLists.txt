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

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} GenericRepository )


CONFIGURE_FILE(
  ${CYCLUS_SOURCE_DIR}/Core${MODEL_PATH}/GenericRepository.rng
  ${PROJECT_BINARY_DIR}/lib${MODEL_PATH}/GenericRepository.rng
  COPYONLY
  )

install(TARGETS GenericRepository
  LIBRARY DESTINATION cyclus/lib${MODEL_PATH}
  COMPONENT ${MODEL_PATH}
  )

SET(RNG_INCLUDES ${RNG_INCLUDES}
  "<include href='../lib${MODEL_PATH}/GenericRepository.rng'/>"
  PARENT_SCOPE)

install(FILES "GenericRepository.rng"
  DESTINATION cyclus/lib${MODEL_PATH}
  COMPONENT "${MODEL_PATH}.rng"
  )

SET(FacilityTestSource ${FacilityTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/GenericRepositoryTests.cpp 
  ${GenericRepository_SRC}
  PARENT_SCOPE)

SET(Facility_REFS ${Facility_REFS}
  "<ref name='GenericRepository'/>"
  PARENT_SCOPE)


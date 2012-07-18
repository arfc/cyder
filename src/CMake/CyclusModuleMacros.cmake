macro(cyclus_init  _path _name)

  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}${_path})

  # Build the libraries from the CYDER_SRC source files
  ADD_LIBRARY( ${_name}       ${_name}.cpp )
  # Link the libraries to libcycluscore 
  TARGET_LINK_LIBRARIES(${_name} dl ${LIBS})
  SET(CYDER_LIBRARIES ${CYDER_LIBRARIES} ${_name} )

  
  CONFIGURE_FILE(
    ${CYDER_SOURCE_DIR}${_path}/${_name}.rng
    ${PROJECT_BINARY_DIR}/lib${_path}/${_name}.rng
    COPYONLY
    )
  
  install(TARGETS ${_name}
    LIBRARY DESTINATION cyclus/lib${_path}
    COMPONENT ${_path}
    )
  
endmacro()
  

macro(cyclus_init_model _type _name)
  SET(MODEL_PATH "/Models/${_type}/${_name}")
  cyclus_init(${MODEL_PATH} ${_name})

  SET(${_type}TestSource ${${_type}TestSource} 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_name}Tests.cpp 
    PARENT_SCOPE)
endmacro()


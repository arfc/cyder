/*! \file ThermalModelFactory.h
  \brief Declares the ThermalModelFactory class which produces ThermalModels
  \author Kathryn D. Huff
 */
#if !defined(_THERMALMODELFACTORY_H)
#define _THERMALMODELFACTORY_H

#include <map>

#include "Geometry.h"
#include "MatDataTable.h"
#include "Material.h"
#include "ThermalModel.h"
#include "QueryEngine.h"

/** 
   @brief A class which selects and produces concrete instances of ThermalModels
 */
class ThermalModelFactory {

public:
  /**
     Creates a concrete ThermalModel from a QueryEngine pointer. This will have
     and uninitialized geometry and material table?

     @param qe query engine pointer describing the thermal model
    */
  static ThermalModelPtr thermalModel(QueryEngine* qe);

  /**
     Creates a concrete ThermalModel from a QueryEngine pointer as well as 
     geometry and material data. 

     @param qe query engine pointer describing the thermal model
     @param mat_table a pointer to the material table
     @param geom a pointer to the Geometry object 
    */
  static ThermalModelPtr thermalModel(QueryEngine* qe, MatDataTablePtr mat_table, 
      GeometryPtr geom);

  /** 
     Creates a copy of a ThermalModel from another ThermalModel as well as 
     geometry and material data.

     @param src the original ThermalModel
     @param mat_table a pointer to the material table
     @param geom a pointer to the Geometry object 
     */
  static ThermalModelPtr thermalModel(ThermalModelPtr src, MatDataTablePtr mat_table, 
      GeometryPtr geom);

protected:
  /** 
     Enumerates a string if it is one of the named ThermalModelTypes

     @param type the name of the ThermalModelType (e.g., StubThermal)
    */
  static ThermalModelType thermalEnum(std::string type_name);

  /// ThermalModelType names list
  static std::string thermal_type_names_[LAST_THERMAL];

};


#endif

/*! \file NuclideModelFactory.h
  \brief Declares the NuclideModelFactory class which produces NuclideModels
  \author Kathryn D. Huff
 */
#if !defined(_NUCLIDEMODELFACTORY_H)
#define _NUCLIDEMODELFACTORY_H

#include <map>

#include "Geometry.h"
#include "MatDataTable.h"
#include "Material.h"
#include "NuclideModel.h"
#include "QueryEngine.h"

/** 
   @brief A class which selects and produces concrete instances of NuclideModels
 */
class NuclideModelFactory {

public:
  /**
     Creates a concrete NuclideModel from a QueryEngine pointer. This will have
     and uninitialized geometry and material table?

     @param qe query engine pointer describing the nuclide model
    */
  static NuclideModelPtr nuclideModel(QueryEngine* qe);

  /**
     Creates a concrete NuclideModel from a QueryEngine pointer as well as 
     geometry and material data. 

     @param qe query engine pointer describing the nuclide model
     @param mat_table a pointer to the material table
     @param geom a pointer to the Geometry object 
     @param comp_id the id number of the owning component
    */
  static NuclideModelPtr nuclideModel(QueryEngine* qe, MatDataTablePtr mat_table, 
      GeometryPtr geom, int comp_id);

  /** 
     Creates a copy of a NuclideModel from another NuclideModel as well as 
     geometry and material data.

     @param src the original NuclideModel
     @param mat_table a pointer to the material table
     @param geom a pointer to the Geometry object 
     @param comp_id the id number of the owning component
     */
  static NuclideModelPtr nuclideModel(NuclideModelPtr src, MatDataTablePtr mat_table, 
      GeometryPtr geom, int comp_id);

protected:
  /** 
     Enumerates a string if it is one of the named NuclideModelTypes

     @param type the name of the NuclideModelType (e.g., StubNuclide)
    */
  static NuclideModelType nuclideEnum(std::string type_name);

  /// NuclideModelType names list
  static std::string nuclide_type_names_[LAST_NUCLIDE];

};


#endif

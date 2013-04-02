/** \file NuclideModelFactory.cpp
 * \brief Implements the NuclideModelFactory class used by the Generic Repository 
 * \author Kathryn D. Huff
 */

#include <boost/lexical_cast.hpp>

#include "NuclideModelFactory.h"
#include "OneDimPPMNuclide.h"
#include "MixedCellNuclide.h"
#include "DegRateNuclide.h"
#include "LumpedNuclide.h"
#include "StubNuclide.h"
#include "Logger.h"
#include "EventManager.h"

using boost::lexical_cast;

// Static variables to be initialized.

std::string NuclideModelFactory::nuclide_type_names_[] = {
  "DegRateNuclide",
  "LumpedNuclide",
  "MixedCellNuclide",
  "OneDimPPMNuclide",
  "StubNuclide", 
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr NuclideModelFactory::nuclideModel(QueryEngine* qe){
  NuclideModelPtr to_ret;


  std::string model_name = qe->getElementName();;
  QueryEngine* input = qe->queryElement(model_name);

  switch(nuclideEnum(model_name))
  {
    case DEGRATE_NUCLIDE:
      to_ret = NuclideModelPtr(DegRateNuclide::create(input));
      break;
    case LUMPED_NUCLIDE:
      to_ret = NuclideModelPtr(LumpedNuclide::create(input));
      break;
    case MIXEDCELL_NUCLIDE:
      to_ret = NuclideModelPtr(MixedCellNuclide::create(input));
      break;
    case ONEDIMPPM_NUCLIDE:
      to_ret = NuclideModelPtr(OneDimPPMNuclide::create(input));
      break;
    case STUB_NUCLIDE:
      to_ret = NuclideModelPtr(StubNuclide::create(input));
      break;
    default:
      throw CycException("Unknown nuclide model enum value encountered."); 
  }

  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr NuclideModelFactory::nuclideModel(QueryEngine* qe, MatDataTablePtr 
    mat_table, GeometryPtr geom){
    NuclideModelPtr to_ret = nuclideModel(qe);
    to_ret->set_mat_table(mat_table);
    to_ret->set_geom(geom);
    return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr NuclideModelFactory::nuclideModel(NuclideModelPtr src, 
    MatDataTablePtr mat_table, GeometryPtr geom){
  NuclideModelPtr to_ret;
  switch(src->type())
  {
    case DEGRATE_NUCLIDE:
      to_ret = NuclideModelPtr(DegRateNuclide::create());
      break;
    case LUMPED_NUCLIDE:
      to_ret = NuclideModelPtr(LumpedNuclide::create());
      break;
    case MIXEDCELL_NUCLIDE:
      to_ret = NuclideModelPtr(MixedCellNuclide::create());
      break;
    case ONEDIMPPM_NUCLIDE:
      to_ret = NuclideModelPtr(OneDimPPMNuclide::create());
      break;
    case STUB_NUCLIDE:
      to_ret = NuclideModelPtr(StubNuclide::create());
      break;
    default:
      throw CycException("Unknown nuclide model enum value encountered when copying."); 
  }      
  to_ret->copy(*src);
  to_ret->set_mat_table(MatDataTablePtr(mat_table));
  to_ret->set_geom(GeometryPtr(geom));
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelType NuclideModelFactory::nuclideEnum(std::string type_name) {
  NuclideModelType to_ret = LAST_NUCLIDE;
  for(int type = 0; type < LAST_NUCLIDE; type++){
    if(nuclide_type_names_[type] == type_name){
      to_ret = (NuclideModelType)type;
    }
  }
  if (to_ret == LAST_NUCLIDE){
    std::string err_msg ="'";
    err_msg += type_name;
    err_msg += "' does not name a valid NuclideModelType.\n";
    err_msg += "Options are:\n";
    for(int name=0; name < LAST_NUCLIDE; name++){
      err_msg += nuclide_type_names_[name];
      err_msg += "\n";
    }
    throw CycException(err_msg);
  }
  return to_ret;
}

/** \file ThermalModelFactory.cpp
 * \brief Implements the ThermalModelFactory class used by the Generic Repository 
 * \author Kathryn D. Huff
 */

#include <boost/lexical_cast.hpp>

#include "ThermalModelFactory.h"
#include "LumpedThermal.h"
#include "STCThermal.h"
#include "StubThermal.h"
#include "Logger.h"
#include "EventManager.h"

using boost::lexical_cast;

// Static variables to be initialized.

std::string ThermalModelFactory::thermal_type_names_[] = {
  "LumpedThermal",
  "STCThermal",
  "StubThermal"
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ThermalModelPtr ThermalModelFactory::thermalModel(QueryEngine* qe){
  ThermalModelPtr to_ret;

  std::string model_name = qe->getElementName();;
  
  switch(thermalEnum(model_name))
  {
    case LUMPED_THERMAL:
      to_ret = ThermalModelPtr(LumpedThermal::create(qe));
      break;
    case STUB_THERMAL:
      to_ret = ThermalModelPtr(StubThermal::create(qe));
      break;
    case STC_THERMAL:
      to_ret = ThermalModelPtr(LumpedThermal::create(qe));
      break;
    default:
      throw CycException("Unknown thermal model enum value encountered."); 
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ThermalModelPtr ThermalModelFactory::thermalModel(QueryEngine* qe, MatDataTablePtr 
    mat_table, GeometryPtr geom){
    ThermalModelPtr to_ret = thermalModel(qe);
    to_ret->set_mat_table(mat_table);
    to_ret->set_geom(geom);
    return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ThermalModelPtr ThermalModelFactory::thermalModel(ThermalModelPtr src, 
    MatDataTablePtr mat_table, GeometryPtr geom){
  ThermalModelPtr to_ret;
  
  switch( src->type() )
  {
    case LUMPED_THERMAL:
      to_ret = ThermalModelPtr(LumpedThermal::create());
      break;
    case STUB_THERMAL:
      to_ret = ThermalModelPtr(StubThermal::create());
      break;
    case STC_THERMAL: 
      to_ret = ThermalModelPtr(LumpedThermal::create());
      break;
    default:
      throw CycException("Unknown thermal model enum value encountered when copying."); 
  }      
  to_ret->copy(src);
  to_ret->set_mat_table(mat_table);
  to_ret->set_geom(geom);
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ThermalModelType ThermalModelFactory::thermalEnum(std::string type_name) {
  ThermalModelType toRet = LAST_THERMAL;
  for(int type = 0; type < LAST_THERMAL; type++){
    if(thermal_type_names_[type] == type_name){
      toRet = (ThermalModelType)type;
    } 
  }
  if (toRet == LAST_THERMAL){
    std::string err_msg ="'";
    err_msg += type_name;
    err_msg += "' does not name a valid ThermalModelType.\n";
    err_msg += "Options are:\n";
    for(int name=0; name < LAST_THERMAL; name++){
      err_msg += thermal_type_names_[name];
      err_msg += "\n";
    }     
    throw CycException(err_msg);
  }
  return toRet;
}

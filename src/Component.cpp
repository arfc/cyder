/** \file Component.cpp
 * \brief Implements the Component class used by the Generic Repository 
 * \author Kathryn D. Huff
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "Component.h"
#include "LumpedThermal.h"
#include "StubThermal.h"
#include "DegRateNuclide.h"
#include "LumpedNuclide.h"
#include "MixedCellNuclide.h"
#include "OneDimPPMNuclide.h"
#include "StubNuclide.h"
#include "TwoDimPPMNuclide.h"
#include "BookKeeper.h"
#include "InputXML.h"
#include "Logger.h"

using namespace std;

// Static variables to be initialized.
int Component::nextID_ = 0;

string Component::thermal_type_names_[] = {
  "LumpedThermal",
  "StubThermal"
};
string Component::nuclide_type_names_[] = {
  "DegRateNuclide",
  "LumpedNuclide",
  "MixedCellNuclide",
  "OneDimPPMNuclide",
  "StubNuclide", 
  "TwoDimPPMNuclide" 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Component::Component(){
  name_ = "";
  geom_.inner_radius_ = 0;  // 0 indicates a solid
  geom_.outer_radius_ = NULL;   // NULL indicates an infinite object
  point_t center = {0,0,0};
  geom_.centroid_ = center; // by default, the origin is the center

  temperature_ = 0;
  temperature_lim_ = 373;
  toxicity_lim_ = 10 ;

  thermal_model_ = NULL;
  nuclide_model_ = NULL;
  parent_component_ = NULL;

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Component::~Component(){ // @TODO is there anything to delete? Make this virtual? 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::init(xmlNodePtr cur){
  ID_=nextID_++;
  
  name_ = XMLinput->get_xpath_content(cur,"name");
  type_ = getComponentType(XMLinput->get_xpath_content(cur,"componenttype"));
  geom_.inner_radius_ = strtod(XMLinput->get_xpath_content(cur,"innerradius"),NULL);
  geom_.outer_radius_ = strtod(XMLinput->get_xpath_content(cur,"outerradius"),NULL);

  thermal_model_ = getThermalModel(cur);
  nuclide_model_ = getNuclideModel(cur);

  parent_component_ = NULL;

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();
  LOG(LEV_DEBUG2,"GRComp") << "The Component Class init(cur) function has been called.";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::copy(Component* src){
  ID_=nextID_++;

  name_ = src->name_;
  type_ = src->type_;

  geom_.inner_radius_ = src->geom_.inner_radius_;
  geom_.outer_radius_ = src->geom_.outer_radius_;

  thermal_model_ = copyThermalModel(src->thermal_model_);
  if (!thermal_model_){
    string err = "The " ;
    err += name_;
    err += " model with ID: ";
    err += ID_;
    err += " does not have a thermal model";
    throw CycException(err);
  }
  nuclide_model_ = copyNuclideModel(src->nuclide_model_);
  parent_component_ = NULL;

  temperature_ = src->temperature_;
  temperature_lim_ = src->temperature_lim_ ;
  toxicity_lim_ = src->toxicity_lim_ ;

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Component::print(){
  LOG(LEV_DEBUG2,"GRComp") << "Component: " << this->name();
  LOG(LEV_DEBUG2,"GRComp") << "Contains Materials:";
  for(int i=0; i<this->getWastes().size() ; i++){
    LOG(LEV_DEBUG2,"GRComp") << wastes_[i];
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::absorb(mat_rsrc_ptr mat_to_add){
  try{
    nuclide_model_->absorb(mat_to_add);
  } catch ( exception& e ) {
    LOG(LEV_ERROR, "GRComp") << "Error occured in component absorb function." << e.what();
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::extract(mat_rsrc_ptr mat_to_rem){
  try{
    nuclide_model_->extract(mat_to_rem);
  } catch ( exception& e ) {
    LOG(LEV_ERROR, "GRComp") << "Error occured in component extract function." << e.what();
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::transportHeat(){
  if ( NULL == thermal_model_ ) {
    LOG(LEV_ERROR, "GRComp") << "Error, no thermal_model_ loaded before Component::transportHeat." ;
  } else {
    thermal_model_->transportHeat();
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::transportNuclides(){
  if ( NULL == nuclide_model_ ) {
    LOG(LEV_ERROR, "GRComp") << "Error, no nuclide_model_ loaded before Component::transportNuclides." ;
  } else { 
    nuclide_model_->transportNuclides();
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Component* Component::load(ComponentType type, Component* to_load) {
  to_load->setParent(this);
  daughter_components_.push_back(to_load);
  return this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Component::isFull() {
  return true; //TEMPORARY
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ComponentType Component::getComponentType(std::string type_name) {
  ComponentType toRet = LAST_EBS;
  string component_type_names[] = {"BUFFER", "ENV", "FF", "NF", "WF", "WP"};
  for(int type = 0; type < LAST_EBS; type++){
    if(component_type_names[type] == type_name){
      toRet = (ComponentType)type;
    } 
  }
  if (toRet == LAST_EBS){
    string err_msg ="'";
    err_msg += type_name;
    err_msg += "' does not name a valid ComponentType.\n";
    err_msg += "Options are:\n";
    for(int name=0; name < LAST_EBS; name++){
      err_msg += component_type_names[name];
      err_msg += "\n";
    }
    throw CycException(err_msg);
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ThermalModelType Component::getThermalModelType(std::string type_name) {
  ThermalModelType toRet = LAST_THERMAL;
  for(int type = 0; type < LAST_THERMAL; type++){
    if(thermal_type_names_[type] == type_name){
      toRet = (ThermalModelType)type;
    } 
  }
  if (toRet == LAST_THERMAL){
    string err_msg ="'";
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelType Component::getNuclideModelType(std::string type_name) {
  NuclideModelType toRet = LAST_NUCLIDE;
  for(int type = 0; type < LAST_NUCLIDE; type++){
    if(nuclide_type_names_[type] == type_name){
      toRet = (NuclideModelType)type;
    }
  }
  if (toRet == LAST_NUCLIDE){
    string err_msg ="'";
    err_msg += type_name;
    err_msg += "' does not name a valid NuclideModelType.\n";
    err_msg += "Options are:\n";
    for(int name=0; name < LAST_NUCLIDE; name++){
      err_msg += nuclide_type_names_[name];
      err_msg += "\n";
    }
    throw CycException(err_msg);
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ThermalModel* Component::getThermalModel(xmlNodePtr cur){
  ThermalModel* toRet;
  string model_name = XMLinput->get_xpath_name(cur,"thermalmodel/*");
  
  switch(getThermalModelType(model_name))
  {
    case LUMPED_THERMAL:
      toRet = new LumpedThermal(cur);
      break;
    case STUB_THERMAL:
      toRet = new StubThermal(cur);
      break;
    default:
      throw CycException("Unknown thermal model enum value encountered."); 
  }
  return toRet;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
NuclideModel* Component::getNuclideModel(xmlNodePtr cur){
  NuclideModel* toRet;

  string model_name = XMLinput->get_xpath_name(cur,"nuclidemodel/*");

  switch(getNuclideModelType(model_name))
  {
    case DEGRATE_NUCLIDE:
      toRet = new DegRateNuclide(cur);
      break;
    case LUMPED_NUCLIDE:
      toRet = new LumpedNuclide(cur);
      break;
    case MIXEDCELL_NUCLIDE:
      toRet = new MixedCellNuclide(cur);
      break;
    case ONEDIMPPM_NUCLIDE:
      toRet = new OneDimPPMNuclide(cur);
      break;
    case STUB_NUCLIDE:
      toRet = new StubNuclide(cur);
      break;
    case TWODIMPPM_NUCLIDE:
      toRet = new TwoDimPPMNuclide(cur);
      break;
    default:
      throw CycException("Unknown nuclide model enum value encountered."); 
  }
  return toRet;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ThermalModel* Component::copyThermalModel(ThermalModel* src){
  ThermalModel* toRet;
  switch( src->getThermalModelType() )
  {
    case LUMPED_THERMAL:
      toRet = new LumpedThermal();
      toRet->copy(src);
      break;
    case STUB_THERMAL:
      toRet = new StubThermal();
      toRet->copy(src);
      break;
    default:
      throw CycException("Unknown thermal model enum value encountered when copying."); 
  }      
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
NuclideModel* Component::copyNuclideModel(NuclideModel* src){
  NuclideModel* toRet;
  switch(src->getNuclideModelType())
  {
    case DEGRATE_NUCLIDE:
      toRet = new DegRateNuclide();
      break;
    case LUMPED_NUCLIDE:
      toRet = new LumpedNuclide();
      break;
    case MIXEDCELL_NUCLIDE:
      toRet = new MixedCellNuclide();
      break;
    case ONEDIMPPM_NUCLIDE:
      toRet = new OneDimPPMNuclide();
      break;
    case STUB_NUCLIDE:
      toRet = new StubNuclide();
      break;
    case TWODIMPPM_NUCLIDE:
      toRet = new TwoDimPPMNuclide();
      break;
    default:
      throw CycException("Unknown nuclide model enum value encountered when copying."); 
  }      
  toRet->copy(src);
  return toRet;
}


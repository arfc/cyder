/*! \file LumpedNuclide.cpp
    \brief Implements the LumpedNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <deque>
#include <time.h>
#include <boost/lexical_cast.hpp>

#include "CycException.h"
#include "Logger.h"
#include "Timer.h"
#include "LumpedNuclide.h"

using namespace std;
using boost::lexical_cast;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedNuclide::LumpedNuclide() : 
  t_t_(0),
  Pe_(0),
  porosity_(0),
  formulation_(LAST_FORMULATION_TYPE)
{ 
  set_geom(GeometryPtr(new Geometry()));
  last_updated_=0;

  t_t_ = 0;
  v_ = 0;
  Pe_ = 0;
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedNuclide::LumpedNuclide(QueryEngine* qe):
  t_t_(0),
  Pe_(0),
  porosity_(0),
  formulation_(LAST_FORMULATION_TYPE)
{ 

  set_geom(GeometryPtr(new Geometry()));
  last_updated_=0;

  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedNuclide::~LumpedNuclide(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void LumpedNuclide::initModuleMembers(QueryEngine* qe){
  t_t_ = lexical_cast<double>(qe->getElementContent("transit_time"));
  v_ = lexical_cast<double>(qe->getElementContent("advective_velocity"));
  porosity_ = lexical_cast<double>(qe->getElementContent("porosity"));

  Pe_=NULL;

  list<string> choices;
  list<string>::iterator it;
  choices.push_back ("DM");
  choices.push_back ("EM");
  choices.push_back ("PFM");

  QueryEngine* formulation_qe = qe->queryElement("formulation");
  string formulation_string;
  for( it=choices.begin(); it!=choices.end(); it++){
    if (formulation_qe->nElementsMatchingQuery(*it) == 1){
      formulation_=enumerateFormulation(*it);
      formulation_string=(*it);
    }
  }
  QueryEngine* ptr = formulation_qe->queryElement(formulation_string);
  switch(formulation_){
    case DM :
      Pe_ = lexical_cast<double>(ptr->getElementContent("peclet"));
      break;
    case EM :
      break;
    case PFM :
      break;
    default:
      string err = "The formulation type '"; 
      err += formulation_;
      err += "' is not supported.";
      throw CycException(err);
      LOG(LEV_ERROR,"GRLNuc") << err;
      break;
  }

  LOG(LEV_DEBUG2,"GRLNuc") << "The LumpedNuclide Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr LumpedNuclide::copy(const NuclideModel& src){
  const LumpedNuclide* src_ptr = dynamic_cast<const LumpedNuclide*>(&src);

  set_last_updated(TI->time());
  set_t_t(src_ptr->t_t());
  set_Pe(src_ptr->Pe());
  set_porosity(src_ptr->porosity());
  set_formulation(src_ptr->formulation());

  // copy the geometry AND the centroid, it should be reset later.
  set_geom(geom_->copy(src_ptr->geom(), src_ptr->geom()->centroid()));

  wastes_ = deque<mat_rsrc_ptr>();
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  update(TI->time());

  return shared_from_this();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::update(int the_time){
  update_vec_hist(the_time);
  update_conc_hist(the_time);
  set_last_updated(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::print(){
    LOG(LEV_DEBUG2,"GRLNuc") << "LumpedNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::absorb(mat_rsrc_ptr matToAdd) {
  // Get the given LumpedNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRLNuc") << "LumpedNuclide is absorbing material: ";
  matToAdd->print();
  wastes_.push_back(matToAdd);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr LumpedNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem) {
  // Get the given LumpedNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRLNuc") << "LumpedNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
  mat_rsrc_ptr to_ret = mat_rsrc_ptr(MatTools::extract(comp_to_rem, kg_to_rem, wastes_));
  update(TI->time());
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::transportNuclides(int the_time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The LumpedNuclide class should transport all nuclides
  update(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> LumpedNuclide::source_term_bc(){
  double tot_mass = 0;
  IsoConcMap conc_map = MatTools::scaleConcMap(conc_hist(last_updated()), V_f());
  CompMapPtr comp_map = CompMapPtr(new CompMap(MASS));
  IsoConcMap::iterator it;
  for( it=conc_map.begin(); it!=conc_map.end(); ++it){
    (*comp_map)[(*it).first]=(*it).second;
    tot_mass += (*it).second;
  }
  IsoVector to_ret = IsoVector(comp_map);
  return make_pair(to_ret, tot_mass);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap LumpedNuclide::dirichlet_bc(){
  return conc_hist(last_updated());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap LumpedNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  ConcGradMap to_ret;
  IsoConcMap c_int = conc_hist(last_updated());
  Radius r_int = geom()->radial_midpoint();

  int iso;
  IsoConcMap::iterator it;
  for( it=c_int.begin(); it!=c_int.end(); ++it){
    iso=(*it).first;
    if( c_ext.count(iso) != 0 ) {
      // in both
      to_ret[iso] = calc_conc_grad(c_ext[iso], c_int[iso], r_ext, r_int);
    } else {
      // in c_int only
      to_ret[iso] = calc_conc_grad(0, c_int[iso], r_ext, r_int);
    }
  }
  for( it=c_ext.begin(); it!=c_ext.end(); ++it){
    iso = (*it).first;
    if( c_int.count(iso) == 0) {
      // in c_ext only
      to_ret[iso] = calc_conc_grad(c_ext[iso], 0, r_ext, r_int);
    }
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoFluxMap LumpedNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  IsoFluxMap to_ret;
  ConcGradMap neumann = neumann_bc(c_ext, r_ext);

  ConcGradMap::iterator it;
  Iso iso;
  Elem elem;
  for( it = neumann.begin(); it!=neumann.end(); ++it){
    iso = (*it).first;
    elem = iso/1000;
    to_ret.insert(make_pair(iso, -mat_table_->D(elem)*(*it).second + shared_from_this()->dirichlet_bc(iso)*v()));
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
FormulationType LumpedNuclide::enumerateFormulation(string type_name) {
  FormulationType toRet = LAST_FORMULATION_TYPE;
  string formulation_type_names[] = {"DM", "EM", "PFM", "LAST_FORMULATION_TYPE"};
  for(int type = 0; type < LAST_FORMULATION_TYPE; type++){
    if(formulation_type_names[type] == type_name){
      toRet = (FormulationType)type;
    } 
  }
  if (toRet == LAST_FORMULATION_TYPE){
    string err_msg ="'";
    err_msg += type_name;
    err_msg += "' does not name a valid FormulationType.\n";
    err_msg += "Options are:\n";
    for(int name=0; name < LAST_FORMULATION_TYPE; name++){
      err_msg += formulation_type_names[name];
      err_msg += "\n";
    }
    throw CycException(err_msg);
  }
  return toRet;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::set_Pe(double Pe){
  // We won't allow the situation in which Pe is negative
  // as it would mean that the flow is moving toward the source, 
  // which won't happen for vertical flow
  if( Pe < 0 ) {
    stringstream msg_ss;
    msg_ss << "The LumpedNuclide Pe range is 0 to infinity, inclusive.";
    msg_ss << " The value provided was ";
    msg_ss << Pe;
    msg_ss <<  ".";
    LOG(LEV_ERROR,"GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  } else {
    Pe_ = Pe;
  }
  MatTools::validate_finite_pos((Pe));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::set_porosity(double porosity){
  try {
    MatTools::validate_percent(porosity);
  } catch (CycRangeException e) {
    stringstream msg_ss;
    msg_ss << "The LumpedNuclide porosity range is 0 to 1, inclusive.";
    msg_ss << " The value provided was ";
    msg_ss << porosity;
    msg_ss <<  ".";
    LOG(LEV_ERROR,"GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  }

  porosity_ = porosity;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double LumpedNuclide::V_ff(){
  return V_f();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double LumpedNuclide::V_f(){
  return MatTools::V_f(V_T(),porosity());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double LumpedNuclide::V_s(){
  return MatTools::V_s(V_T(),porosity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double LumpedNuclide::V_T(){
  return geom_->volume();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::update_conc_hist(int the_time, deque<mat_rsrc_ptr> mats){
  assert(last_updated() <= the_time);
  IsoConcMap to_ret;

  pair<IsoVector, double> sum_pair;
  sum_pair = vec_hist_[the_time];
  IsoConcMap C_0 = MatTools::comp_to_conc_map(sum_pair.first.comp(), sum_pair.second, V_f());

  switch(formulation_){
    case DM :
      to_ret = C_DM(C_0, the_time);
      break;
    case EM :
      to_ret = C_EM(C_0, the_time);
      break;
    case PFM :
      to_ret = C_PFM(C_0, the_time);
      break;
    default:
      string err = "The formulation type '"; 
      err += formulation_;
      err += "' is not supported.";
      throw CycException(err);
      LOG(LEV_ERROR,"GRLNuc") << err;
      break;
  }
  conc_hist_[the_time] = to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::update_conc_hist(int the_time){
  return update_conc_hist(the_time, wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap LumpedNuclide::C_DM(IsoConcMap C_0, int the_time){
  double arg = (Pe()/2.0)*(1-pow(1+4*the_time/Pe(), 0.5));
  return MatTools::scaleConcMap(C_0, exp(arg));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap LumpedNuclide::C_EM(IsoConcMap C_0, int the_time){
  double scale = 1.0/(1.0+the_time);
  return MatTools::scaleConcMap(C_0, scale);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap LumpedNuclide::C_PFM(IsoConcMap C_0, int the_time){
  double scale = exp(-the_time);
  return MatTools::scaleConcMap(C_0, scale);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::update_vec_hist(int the_time){
  vec_hist_[the_time] = MatTools::sum_mats(wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::update_inner_bc(int the_time, std::vector<NuclideModelPtr> daughters){

  std::vector<NuclideModelPtr>::iterator daughter;
  
  pair<IsoVector, double> st;
  pair<IsoVector, double> mixed;
  Volume vol;
  Volume vol_sum;
  
  for( daughter = daughters.begin(); daughter!=daughters.end(); ++daughter){
    st = (*daughter)->source_term_bc();
    vol = (*daughter)->V_ff();
    vol_sum += vol;
    if(mixed.second==0){
      mixed.first=st.first;
      mixed.second=st.second;
    } else {
      mixed.second +=st.second;
      mixed.first.mix(st.first,mixed.second/st.second);
    }
  }
  C_0_= MatTools::comp_to_conc_map(mixed.first.comp(), mixed.second, vol_sum); 
}




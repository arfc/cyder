/*! \file OneDimPPMNuclide.cpp
    \brief Implements the OneDimPPMNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <deque>
#include <time.h>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/erf.hpp>

#include "CycException.h"
#include "Logger.h"
#include "Timer.h"
#include "OneDimPPMNuclide.h"

using namespace std;
using boost::lexical_cast;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OneDimPPMNuclide::OneDimPPMNuclide():
  Ci_(0),
  Co_(0),
  v_(0),
  porosity_(0),
  rho_(0)
{
  set_geom(GeometryPtr(new Geometry()));
  last_updated_=0;

  wastes_ = deque<mat_rsrc_ptr>();
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OneDimPPMNuclide::OneDimPPMNuclide(QueryEngine* qe):
  Ci_(0),
  Co_(0),
  v_(0),
  porosity_(0),
  rho_(0)
{
  wastes_ = deque<mat_rsrc_ptr>();
  set_geom(GeometryPtr(new Geometry()));
  last_updated_=0;
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OneDimPPMNuclide::~OneDimPPMNuclide(){ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OneDimPPMNuclide::initModuleMembers(QueryEngine* qe){
  // C(x,0)=C_i
  Ci_ = lexical_cast<double>(qe->getElementContent("initial_concentration"));
  // -D{\frac{\partial C}{\partial x}}|_{x=0} + vC = vC_0, for t<t_0
  Co_ = lexical_cast<double>(qe->getElementContent("source_concentration"));
  // advective velocity (hopefully the same as the whole system).
  v_ = lexical_cast<double>(qe->getElementContent("advective_velocity"));
  // rock parameters
  porosity_ = lexical_cast<double>(qe->getElementContent("porosity"));
  rho_ = lexical_cast<double>(qe->getElementContent("bulk_density"));

  LOG(LEV_DEBUG2,"GR1DNuc") << "The OneDimPPMNuclide Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr OneDimPPMNuclide::copy(const NuclideModel& src){
  const OneDimPPMNuclide* src_ptr = dynamic_cast<const OneDimPPMNuclide*>(&src);

  set_last_updated(TI->time());
  set_porosity(src_ptr->porosity());
  set_rho(src_ptr->rho());
  set_v(src_ptr->v());
  set_Ci(src_ptr->Ci());
  set_Co(src_ptr->Co());


  // copy the geometry AND the centroid. It should be reset later.
  set_geom(geom_->copy(src_ptr->geom(), src_ptr->geom()->centroid()));

  wastes_ = deque<mat_rsrc_ptr>();
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  update_vec_hist(TI->time());

  return shared_from_this();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::print(){
    LOG(LEV_DEBUG2,"GR1DNuc") << "OneDimPPMNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given OneDimPPMNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GR1DNuc") << "OneDimPPMNuclide is absorbing material: ";
  matToAdd->print();
  wastes_.push_back(matToAdd);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr OneDimPPMNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem)
{
  // Get the given OneDimPPMNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GR1DNuc") << "OneDimPPMNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
  mat_rsrc_ptr to_ret = mat_rsrc_ptr(MatTools::extract(comp_to_rem, kg_to_rem, wastes_));
  update(last_updated());
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::transportNuclides(int the_time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The OneDimPPMNuclide class should transport all nuclides
  update(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::update(int the_time){
  update_vec_hist(the_time);
  update_conc_hist(the_time, wastes_);
  set_last_updated(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> OneDimPPMNuclide::source_term_bc(){
  double tot_mass=0;
  /// @TODO The conc map should be integrated over the volume?
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
IsoConcMap OneDimPPMNuclide::dirichlet_bc(){
  return conc_hist(last_updated());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap OneDimPPMNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  ConcGradMap to_ret;
  IsoConcMap c_int = conc_hist(last_updated());
  Radius r_int = geom()->radial_midpoint();

  int iso;
  IsoConcMap::iterator it;
  for( it=c_int.begin(); it!=c_int.end(); ++it ){
    iso=(*it).first;
    if(c_ext.count(iso) != 0 ){
      // in both
      to_ret[iso] = calc_conc_grad(c_ext[iso], c_int[iso], r_ext, r_int);
    } else { 
      // in c_int only
      to_ret[iso] = calc_conc_grad(0, c_int[iso], r_ext, r_int);
    }
  }
  for( it=c_ext.begin(); it!=c_ext.end(); ++it){
    iso = (*it).first;
    if( c_int.count(iso) == 0 ){
      // in c_ext only
      to_ret[iso] = calc_conc_grad(c_ext[iso], 0, r_ext, r_int);
    }
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoFluxMap OneDimPPMNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  IsoFluxMap to_ret;
  ConcGradMap neumann = neumann_bc(c_ext,r_ext);

  ConcGradMap::iterator it;
  Iso iso;
  Elem elem;
  for( it=neumann.begin(); it!=neumann.end(); ++it){
    iso=(*it).first;
    elem=iso/1000;
    to_ret.insert(make_pair(iso, -mat_table_->D(elem)*(*it).second + shared_from_this()->dirichlet_bc(iso)*v()));
  }

  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::update_vec_hist(int the_time){
  vec_hist_[the_time]=MatTools::sum_mats(wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::update_conc_hist(int the_time){
  return update_conc_hist(the_time, wastes_);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::update_conc_hist(int the_time, deque<mat_rsrc_ptr> mats){
  assert(last_updated() <= the_time);
  IsoConcMap to_ret;

  pair<IsoVector, double> sum_pair;
  IsoConcMap C_0 = MatTools::comp_to_conc_map(sum_pair.first.comp(), sum_pair.second, V_f());

  /// @TODO this is a placeholder and only calculates C at the midpoint
  /// @TODO this doesn't actually use the mats... 
  Radius r_calc = geom_->radial_midpoint();
  to_ret = conc_profile(C_0, r_calc, the_time);
  set_last_updated(the_time);
  conc_hist_[the_time] = to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap OneDimPPMNuclide::conc_profile(IsoConcMap C_0, Radius r, int dt){

  // @TODO decay will accidentally get neglected here if you don't watch out. Fix.
  IsoConcMap to_ret;
  IsoConcMap::iterator it;
  Iso iso;
  for(it=C_0.begin(); it!=C_0.end(); ++it){
    iso = (*it).first;
    to_ret[iso] = calculate_conc(C_0, r, iso, dt);
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::calculate_conc(IsoConcMap C_0, double r, Iso iso, int dt) {
  double D_L = mat_table_->D(iso/1000);
  double pi = boost::math::constants::pi<double>();
  double term_1_frac = (r-v_*dt)/2*pow(D_L*dt,0.5);
  double term_1_scalar = boost::math::erfc(term_1_frac);
  double term_2_radical = (pow(v_,2)*dt/pi/D_L);
  double term_2_exp = exp( -pow(r-v_*dt,2)/(4*D_L*dt)); 
  double term_2_scalar = 0.5*pow(term_2_radical,0.5)*term_2_exp;
  double term_3_factor = 0.5*(1 + v_*r/D_L + pow(v_,2)*dt/D_L);
  double term_3_exp = exp(v_*r/D_L);
  double term_3_erfc = boost::math::erfc( (r - v_*dt) / (2*pow(D_L*dt,0.5)) );
  double term_3_scalar = 0.5*term_3_factor*term_3_exp*term_3_erfc;
  double scalar = term_1_scalar + term_2_scalar + term_3_scalar;
  return C_0[iso]*0.5*scalar;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_porosity(double porosity){
  try { 
    MatTools::validate_percent(porosity);
  } catch (CycRangeException e) {
    stringstream msg_ss;
    msg_ss << "The OneDimPPMNuclide porosity range is 0 to 1, inclusive.";
    msg_ss << " The value provided was ";
    msg_ss << porosity;
    msg_ss << ".";
    LOG(LEV_ERROR, "GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  }
  porosity_ = porosity;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_rho(double rho){
  try { 
    MatTools::validate_finite_pos(rho);
  } catch (CycRangeException e) {
    stringstream msg_ss;
    msg_ss << "The OneDimPPMNuclide bulk density (rho) range must be positive and finite";
    msg_ss << " The value provided was ";
    msg_ss << rho;
    msg_ss << ".";
    LOG(LEV_ERROR, "GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  }
  rho_ = rho;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_Co(double Co){
  Co_=Co;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_Ci(double Ci){
  Ci_=Ci;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_v(double v){
  v_=v;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::V_T(){
  return geom_->volume();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::V_f(){
  return MatTools::V_f(V_T(), porosity());
}

void OneDimPPMNuclide::update_inner_bc(int the_time, std::vector<NuclideModelPtr> daughters){
  /// @TODO use cauchy.
  std::map<NuclideModelPtr, std::pair<IsoVector,double> > to_ret;
  std::vector<NuclideModelPtr>::iterator daughter;
  std::pair<IsoVector, double> source_term;
  for( daughter = daughters.begin(); daughter!=daughters.end(); ++daughter){
    source_term = (*daughter)->source_term_bc();
    if( source_term.second > 0 ){
      absorb((*daughter)->extract(source_term.first.comp(), source_term.second));
    }
  }
}



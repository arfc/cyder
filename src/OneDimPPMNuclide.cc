/*! \file OneDimPPMNuclide.cpp
    \brief Implements the OneDimPPMNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <deque>
#include <vector>
#include <time.h>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/erf.hpp>

#include "CycException.h"
#include "CycArithmetic.h"
#include "Logger.h"
#include "Timer.h"
#include "OneDimPPMNuclide.h"

using namespace std;
using boost::lexical_cast;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OneDimPPMNuclide::OneDimPPMNuclide():
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

  set_last_updated(0);
  set_porosity(src_ptr->porosity());
  set_rho(src_ptr->rho());
  set_v(src_ptr->v());


  // copy the geometry AND the centroid. It should be reset later.
  set_geom(geom_->copy(src_ptr->geom(), src_ptr->geom()->centroid()));

  wastes_ = deque<mat_rsrc_ptr>();
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  update_vec_hist(TI->time());

  return shared_from_this();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::updateNuclideParamsTable(){
  shared_from_this()->addRowToNuclideParamsTable("porosity", porosity());
  shared_from_this()->addRowToNuclideParamsTable("bulk_density", rho());
  shared_from_this()->addRowToNuclideParamsTable("advective_velocity", v());
  shared_from_this()->addRowToNuclideParamsTable("ref_disp", mat_table_->ref_disp());
  shared_from_this()->addRowToNuclideParamsTable("ref_kd", mat_table_->ref_kd());
  shared_from_this()->addRowToNuclideParamsTable("ref_sol", mat_table_->ref_sol());
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
  return MatTools::sum_mats(wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap OneDimPPMNuclide::dirichlet_bc(){
  // @TODO : should calculate C at r.
  pair<IsoVector, double>sum_pair = source_term_bc(); 
  CompMapPtr comp_map = CompMapPtr(sum_pair.first.comp());
  double mass = sum_pair.second;
  IsoConcMap to_ret;
  to_ret[92235] =0;
  CompMap::const_iterator it;
  for( it=(*comp_map).begin(); it!=(*comp_map).end(); ++it){
    if(V_ff() >0 ){
      to_ret[(*it).first]= mass*(*it).second/(V_ff());
    }
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap OneDimPPMNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  ConcGradMap to_ret;
  IsoConcMap c_int = dirichlet_bc();
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

  pair<IsoVector, double> sum_pair = MatTools::sum_mats(mats);
  IsoConcMap C_0 = MatTools::comp_to_conc_map(sum_pair.first.comp(), sum_pair.second, V_f());

  Radius r_calc = geom_->radial_midpoint();
  set_last_updated(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap OneDimPPMNuclide::calculate_conc_diff(IsoConcMap C_0, IsoConcMap C_i, double r, int t0, int t) {
  IsoConcMap::const_iterator it;
  int iso;
  IsoConcMap to_ret = C_i;
  for(it=C_0.begin(); it!=C_0.end(); ++it){
    iso=(*it).first;
    to_ret[iso] = calculate_conc_diff(C_0, C_i, r, iso, t0, t);
    MatTools::validate_finite_pos(to_ret[iso]);
  }
  return to_ret;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::A1(double R, double z, double v, double t, double D, double L){
  MatTools::validate_finite_pos(R);
  MatTools::validate_nonzero(R);
  MatTools::validate_finite_pos(D);
  MatTools::validate_nonzero(D);

  double erfc_arg = (R*z - v*t)/(2*pow(D*R*t, 0.5));
  return 0.5*boost::math::erfc(erfc_arg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::A2(double R, double z, double v, double t, double D, double L){
  double pi = boost::math::constants::pi<double>();
  MatTools::validate_finite_pos(R);
  MatTools::validate_nonzero(R);
  MatTools::validate_finite_pos(D);
  MatTools::validate_nonzero(D);

  double scalar = pow(v*v*t/(pi*R*D),0.5);
  double exp_arg = -pow(R*z - v*t, 2)/(4*D*R*t);
  return scalar*exp(exp_arg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::A3(double R, double z, double v, double t, double D, double L){
  MatTools::validate_finite_pos(R);
  MatTools::validate_nonzero(R);
  MatTools::validate_finite_pos(D);
  MatTools::validate_nonzero(D);

  double scalar = -0.5*(1 + v*z/D + pow(v,2)*t/(D*R));
  double exp_arg = (v*z)/D;
  double erfc_arg = (R*z + v*t)/(2*pow(D*R*t, 0.5));
  return scalar*exp(exp_arg)*boost::math::erfc(erfc_arg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::A4(double R, double z, double v, double t, double D, double L){
  double pi = boost::math::constants::pi<double>();
  MatTools::validate_finite_pos(R);
  MatTools::validate_nonzero(R);
  MatTools::validate_finite_pos(D);
  MatTools::validate_nonzero(D);

  double root_factor = pow(4*v*v*t/(pi*R*D), 0.5);
  double sum_factor = 1 + (v/(4*D))*(2*L - z + v*t/R) ;
  double scalar = root_factor*sum_factor;

  double exp_arg = (v*L)/D - (R/(4*D*t))*pow(2*L - z + v*t/R, 2) ;

  return scalar*exp(exp_arg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::A5(double R, double z, double v, double t, double D, double L){
  MatTools::validate_finite_pos(R);
  MatTools::validate_nonzero(R);
  MatTools::validate_finite_pos(D);
  MatTools::validate_nonzero(D);

  double sum_factor = 2*L - z + 3*v*t/(2*R) + (v/(4*D))*pow(2*L - z + v*t/R, 2);
  double scalar = -(v/D)*sum_factor;

  double exp_arg = v*L/D;
  double erfc_arg = (R*(2*L - z) + v*t)/2*pow(D*R*t, 0.5) ;

  return scalar*exp(exp_arg)*boost::math::erfc(erfc_arg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::Azt(double R, double z, double v, double t, double D, double L){
  vector<double> terms;
  terms.push_back(A1(R, z, v, t, D, L));
  terms.push_back(A2(R, z, v, t, D, L));
  terms.push_back(A3(R, z, v, t, D, L));
  terms.push_back(A4(R, z, v, t, D, L));
  terms.push_back(A5(R, z, v, t, D, L));

  double to_ret = CycArithmetic::KahanSum(terms);
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::calculate_conc(IsoConcMap C_0, IsoConcMap C_i, double r, Iso iso, int t0, int t) {
  double D = mat_table_->D(iso/1000);
  double L = geom_->outer_radius() - geom_->inner_radius();
  MatTools::validate_finite_pos(D);
  //@TODO add sorption to this model. For now, R=1, no sorption. 
  double R=1;
  assert(t0<t);
  double del_t = t-t0;
  t = 120*SECSPERMONTH * del_t ;
  double A = Azt(R, r, v(), t, D, L);
  double Ci_iso =0;
  if(C_i.find(iso) != C_i.end()) {
    Ci_iso = C_i[iso];
    MatTools::validate_finite_pos(Ci_iso);
  } 
  double C0_iso =0;
  if(C_0.find(iso)!=C_0.end()) {
    C0_iso = C_0[iso];
    MatTools::validate_finite_pos(Ci_iso);
  }
  double to_ret=0;
  to_ret = Ci_iso + (C0_iso - Ci_iso)*A ; 
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclide::calculate_conc_diff(IsoConcMap C_0, IsoConcMap C_i, double r, Iso iso, int t0, int t) {
  double to_ret = calculate_conc(C_0, C_i, r, iso, t0, t);
  if(C_i.find(iso) != C_i.end()) {
    to_ret -= C_i[iso];
  } 
  if(to_ret < 0) {
    to_ret =0;
  }
  MatTools::validate_finite_pos(to_ret);
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::update_inner_bc(int the_time, std::vector<NuclideModelPtr> daughters){
  int n=100;
  double a=geom()->inner_radius();
  double b=geom()->outer_radius();
  assert(a<b);
  if(the_time > 0 ) {
    vector<double> calc_points = MatTools::linspace(a,b,n);
    std::vector<NuclideModelPtr>::iterator daughter;
    for(daughter=daughters.begin(); daughter!=daughters.end(); ++daughter){
      std::map<double, IsoConcMap> f_map;
      // m(tn-1) = current contaminants
      std::pair<IsoVector, double> m_prev = MatTools::sum_mats(wastes_);
      // Ci = C(tn-1)
      vector<double>::const_iterator pt;
      for(pt = calc_points.begin(); pt!=calc_points.end(); ++pt){ 
        IsoConcMap C0 = Co(*daughter);
        assert(C0.size()!=0);
        double r = (*pt);
        MatTools::validate_finite_pos(r);
        assert(r<=b);
        assert(r>=a);
        IsoConcMap diff = calculate_conc_diff(C0, Ci(), r-a, the_time-1, 
            the_time);
        f_map.insert(make_pair(r,diff));
      }
      // m(tn) = integrate conc diff
      IsoConcMap to_ret = trap_rule(a, b, n-1, f_map);
      // note, we are using the daughter's volume for safety
      // @TODO use this v_ff after checking appropriateness.
      pair<CompMapPtr, double> m_ij = MatTools::conc_to_comp_map(to_ret, (*daughter)->V_ff());

      stringstream msg_ss;
      msg_ss << "component : ";
      msg_ss << comp_id_;
      msg_ss << " is extracting "; 
      msg_ss << m_ij.second;
      msg_ss << " kg from component ";
      msg_ss << (*daughter)->comp_id();
      msg_ss << " at timestep ";
      msg_ss << TI->time();
      LOG(LEV_DEBUG2, "GRDRNuc") << msg_ss.str();;
      absorb(mat_rsrc_ptr((*daughter)->extract(m_ij.first, 
              m_ij.second)));
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap OneDimPPMNuclide::trap_rule(double a, double b, int n, map<double, IsoConcMap> f_map) {
  double h = (b-a)/n;

  vector<IsoConcMap> terms;
  IsoConcMap f_0;
  IsoConcMap f_n;
  assert(f_map.size() == n+1 );
  assert(f_map.size() >= 2 );

  map<double, IsoConcMap>::iterator it = f_map.find(a);
  if(it!=f_map.end()){ 
    f_0 = MatTools::scaleConcMap((*it).second, h/2.0);
    f_map.erase(it);
  }
  it = f_map.find(b);
  if(it!=f_map.end()){ 
    f_n = MatTools::scaleConcMap((*it).second, h/2.0);
    f_map.erase(it);
  }
  //((h/2)*(f_0+f_n));
  terms.push_back(MatTools::addConcMaps(f_0, f_n));
  map<double,IsoConcMap>::const_iterator f;
  for(f=f_map.begin(); f!=f_map.end(); ++f){
    double r =(*f).first;
    //(h*(f_i));
    terms.push_back( MatTools::scaleConcMap((*f).second, h) );
  }
  IsoConcMap to_ret; 
  IsoConcMap prev;
  vector<IsoConcMap>::const_iterator t;
  for(t=terms.begin(); t!=terms.end(); ++t){
    to_ret = MatTools::addConcMaps(prev, (*t));
    prev = to_ret;
  }
  return to_ret;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_porosity(double porosity){
  try { 
    MatTools::validate_percent(porosity);
  } catch (CycRangeException& e) {
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
  } catch (CycRangeException& e) {
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
IsoConcMap OneDimPPMNuclide::Co(const NuclideModelPtr& daughter) {
  IsoFluxMap dirichlet = daughter->dirichlet_bc();
  return dirichlet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap OneDimPPMNuclide::Ci() {
  pair<IsoVector, double> st = MatTools::sum_mats(wastes_);
  return MatTools::comp_to_conc_map(st.first.comp(), st.second, V_ff());
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

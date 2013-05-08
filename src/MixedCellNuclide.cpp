/*! \file MixedCellNuclide.cpp
    \brief Implements the MixedCellNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <deque>
#include <time.h>
#include <assert.h>
#include <boost/lexical_cast.hpp>

#include "CycException.h"
#include "CycLimits.h"
#include "Logger.h"
#include "Timer.h"
#include "MixedCellNuclide.h"
#include "Material.h"
#include "SolLim.h"

using namespace std;
using boost::lexical_cast;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::MixedCellNuclide():
  bc_type_(LAST_BC_TYPE),
  deg_rate_(0),
  tot_deg_(0),
  last_degraded_(-1),
  v_(0),
  porosity_(0),
  sol_limited_(true),
  kd_limited_(true)
{
  wastes_ = deque<mat_rsrc_ptr>();
  set_geom(GeometryPtr(new Geometry()));
  last_updated_=0;
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::MixedCellNuclide(QueryEngine* qe) : 
  bc_type_(LAST_BC_TYPE),
  deg_rate_(0),
  tot_deg_(0),
  last_degraded_(-1),
  v_(0),
  porosity_(0),
  sol_limited_(true),
  kd_limited_(true)
{
  wastes_ = deque<mat_rsrc_ptr>();
  set_geom(GeometryPtr(new Geometry()));
  last_updated_=0;
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::~MixedCellNuclide(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MixedCellNuclide::initModuleMembers(QueryEngine* qe){
  set_v(lexical_cast<double>(qe->getElementContent("advective_velocity")));
  set_deg_rate(lexical_cast<double>(qe->getElementContent("degradation")));
  set_kd_limited(lexical_cast<bool>(qe->getElementContent("kd_limited")));
  set_porosity(lexical_cast<double>(qe->getElementContent("porosity")));
  set_sol_limited(lexical_cast<bool>(qe->getElementContent("sol_limited")));
  QueryEngine* bc_type_qe = qe->queryElement("bc_type");
  list <string> choices;
  list <string>::iterator it;
  choices.push_back("CAUCHY");
  choices.push_back("DIRICHLET");
  choices.push_back("NEUMANN");
  choices.push_back("SOURCE_TERM");
  for( it=choices.begin(); it!=choices.end(); ++it){
    if( bc_type_qe->nElementsMatchingQuery(*it) == 1 ) {
      set_bc_type(enumerateBCType(*it));
    }
  }
  LOG(LEV_DEBUG2,"GRDRNuc") << "The MixedCellNuclide Class initModuleMembers(qe) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr MixedCellNuclide::copy(const NuclideModel& src){
  const MixedCellNuclide* src_ptr = dynamic_cast<const MixedCellNuclide*>(&src);

  set_v(src_ptr->v());
  set_deg_rate(src_ptr->deg_rate());
  set_kd_limited(src_ptr->kd_limited());
  set_porosity(src_ptr->porosity());
  set_sol_limited(src_ptr->sol_limited());
  set_bc_type(src_ptr->bc_type());
  set_tot_deg(0);
  set_last_degraded(-1);

  // copy the geometry AND the centroid. It should be reset later.
  set_geom(GeometryPtr(new Geometry()));
  geom_->copy(src_ptr->geom(), src_ptr->geom()->centroid());

  wastes_ = deque<mat_rsrc_ptr>();
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();

  return shared_from_this();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::updateNuclideParamsTable(){
  shared_from_this()->addRowToNuclideParamsTable("advective_velocity", v());
  shared_from_this()->addRowToNuclideParamsTable("degradation", deg_rate());
  shared_from_this()->addRowToNuclideParamsTable("kd_limited", double(kd_limited()));
  shared_from_this()->addRowToNuclideParamsTable("porosity", porosity());
  shared_from_this()->addRowToNuclideParamsTable("sol_limited", double(sol_limited()));
  shared_from_this()->addRowToNuclideParamsTable("ref_disp", mat_table_->ref_disp());
  shared_from_this()->addRowToNuclideParamsTable("ref_kd", mat_table_->ref_kd());
  shared_from_this()->addRowToNuclideParamsTable("ref_sol", mat_table_->ref_sol());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::update(int the_time) {
  update_vec_hist(the_time);
  update_conc_hist(the_time);
  set_last_updated(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::print(){
    LOG(LEV_DEBUG2,"GRDRNuc") << "MixedCellNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given MixedCellNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "MixedCellNuclide is absorbing material: ";
  matToAdd->print();
  wastes_.push_back(matToAdd);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr MixedCellNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem)
{
  // Get the given MixedCellNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "MixedCellNuclide " << " is extracting composition: ";
  comp_to_rem->print() ;
  mat_rsrc_ptr to_ret = mat_rsrc_ptr(MatTools::extract(comp_to_rem, kg_to_rem, wastes_));
  update(last_updated());
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::transportNuclides(int the_time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  update_degradation(the_time, deg_rate());
  update(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::set_deg_rate(double cur_rate){
  if( cur_rate < 0 || cur_rate > 1 ) {
    stringstream msg_ss;
    msg_ss << "The MixedCellNuclide degradation rate range is 0 to 1, inclusive.";
    msg_ss << " The value provided was ";
    msg_ss << cur_rate;
    msg_ss <<  ".";
    LOG(LEV_ERROR,"GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  } else {
    deg_rate_ = cur_rate;
  }
  assert((cur_rate >=0) && (cur_rate <= 1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::set_porosity(double porosity){
  if( porosity < 0 || porosity > 1 ) {
    stringstream msg_ss;
    msg_ss << "The MixedCellNuclide porosity range is 0 to 1, inclusive.";
    msg_ss << " The value provided was ";
    msg_ss << porosity;
    msg_ss <<  ".";
    LOG(LEV_ERROR,"GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  } else {
    porosity_ = porosity;
  }
  assert((porosity >=0) && (porosity <= 1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::contained_mass(){
  return shared_from_this()->contained_mass(last_degraded());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> MixedCellNuclide::source_term_bc(){
  //pair<CompMapPtr, double> comp_pair = 
  //  MatTools::conc_to_comp_map(conc_hist(last_degraded()), V_ff());

  int the_time = last_degraded();
  pair<IsoVector, double> sum_pair; 
  sum_pair = MatTools::sum_mats(wastes_);
  CompMapPtr to_ret;
  to_ret = CompMapPtr(new CompMap(MASS));
  double m_tot=0;

  if(sum_pair.second > 0 && V_ff()!=0 && geom_->volume() != numeric_limits<double>::infinity()) { 
    int iso(0);
    double m_ff(0);
    double m_aff(0);
    double mass(sum_pair.second);
    CompMapPtr curr_comp = sum_pair.first.comp();
    curr_comp->massify();
    CompMap::const_iterator it;
    it=(*curr_comp).begin();
    while(it != (*curr_comp).end() ) {
      if( (*it).second >= cyclus::eps_rsrc() ){
        iso = (*it).first;
        m_ff = sorb(the_time, iso, (*it).second*mass);
        m_aff = precipitate(the_time, iso, m_ff);
        (*to_ret)[iso] = m_aff;
        m_tot += m_aff;
      }
      ++it;
    }
  } else {
    (*to_ret)[ 92235 ] = 0; 
  }

  to_ret->massify();
  return make_pair(IsoVector(to_ret), m_tot);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MixedCellNuclide::dirichlet_bc(){
  IsoConcMap c_ff;
  pair<IsoVector, double> source_term = source_term_bc();
  double m_ff = source_term.second;
  c_ff = MatTools::comp_to_conc_map(CompMapPtr(source_term.first.comp()), m_ff, V_ff());
  return c_ff;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap MixedCellNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  ConcGradMap to_ret;

  IsoConcMap c_int;
  pair<IsoVector, double> source_term = shared_from_this()->source_term_bc();
  double m_ff = source_term.second;
  c_int = MatTools::comp_to_conc_map(CompMapPtr(source_term.first.comp()), m_ff, V_ff());
  Radius r_int = geom()->radial_midpoint();
  
  int iso;
  IsoConcMap::iterator it;
  for( it=c_int.begin(); it != c_int.end(); ++it){
    iso = (*it).first;
    Elem elem = int(iso/1000.);
    if( c_ext.count(iso) != 0) {  
      // in both
      to_ret[iso] = calc_conc_grad(c_ext[iso], c_int[iso], r_ext, r_int);
    } else {
      // in c_int_only
      to_ret[iso] = calc_conc_grad(0, c_int[iso], r_ext, r_int);
    }
  }
  for( it=c_ext.begin(); it != c_ext.end(); ++it){
    iso = (*it).first;
    Elem elem = int(iso/1000.);
    if( c_int.count(iso) == 0) { 
      // in c_ext only
      to_ret[iso] = calc_conc_grad(c_ext[iso], 0, r_ext, r_int);
    }
  }

  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoFluxMap MixedCellNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  // -D dC/dx + v_xC = v_x C
  IsoFluxMap to_ret;
  ConcGradMap neumann = neumann_bc(c_ext, r_ext);
  ConcGradMap::iterator it;
  Iso iso;
  Elem elem;
  for( it = neumann.begin(); it != neumann.end(); ++it){
    iso = (*it).first;
    elem = int(iso/1000);
    to_ret.insert(make_pair(iso, -mat_table_->D(elem)*(*it).second + v()*shared_from_this()->dirichlet_bc(iso)));
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MixedCellNuclide::update_conc_hist(int the_time){
  return update_conc_hist(the_time, wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MixedCellNuclide::update_conc_hist(int the_time, deque<mat_rsrc_ptr> mats){
  assert(last_degraded() <= the_time);

  pair<IsoVector, double> sum_pair = source_term_bc(); 

  IsoConcMap to_ret; 

  if(sum_pair.second != 0 && V_ff()!=0 && geom_->volume() != numeric_limits<double>::infinity()) { 
    double mass(sum_pair.second);
    CompMapPtr curr_comp = sum_pair.first.comp();
    curr_comp->massify();
    to_ret = MatTools::comp_to_conc_map(curr_comp, mass, V_ff());
  } else {
    to_ret[ 92235 ] = 0; 
  }
  conc_hist_[the_time] = to_ret ;

  return conc_hist_[the_time];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::update_degradation(int the_time, double cur_rate){
  assert(last_degraded() <= the_time);
  if(last_degraded() == -1 ) { 
    // do nothing, this is the first timestep
    set_tot_deg(0);
  } else {
    if(cur_rate != deg_rate()){
      set_deg_rate(cur_rate);
    };
    double total = tot_deg() + deg_rate()*(the_time - last_degraded());
    set_tot_deg(min(1.0, total));
  }
  set_last_degraded(the_time);

  return tot_deg_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::update_vec_hist(int the_time){
  vec_hist_[ the_time ] = MatTools::sum_mats(wastes_) ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::update_inner_bc(int the_time, std::vector<NuclideModelPtr> daughters){
  std::vector<NuclideModelPtr>::iterator daughter;
  std::pair<IsoVector, double> source_term;
  pair<CompMapPtr, double> comp_pair;
  CompMapPtr comp_to_ext;
  double kg_to_ext;

  for( daughter = daughters.begin(); daughter!=daughters.end(); ++daughter){
    switch (bc_type_) {
      case SOURCE_TERM :
        source_term = (*daughter)->source_term_bc();
        if( source_term.second >= 0 ){
          comp_to_ext = CompMapPtr(source_term.first.comp());
          kg_to_ext=source_term.second;
        }
        break;
      case DIRICHLET :
        comp_pair = inner_dirichlet(*daughter);
        comp_to_ext = CompMapPtr(comp_pair.first);
        kg_to_ext = comp_pair.second;
        break;
      case NEUMANN :
        comp_pair = inner_neumann(*daughter);
        comp_to_ext = CompMapPtr(comp_pair.first);
        kg_to_ext = comp_pair.second;
        break;
      case CAUCHY :
        comp_pair = inner_cauchy(*daughter);
        comp_to_ext = CompMapPtr(comp_pair.first);
        kg_to_ext = comp_pair.second;
        break;
      default :
        std::stringstream err;
        err <<  "The BCType '";
        err << bc_type_;
        err << "' is not valid in the MixedCellNuclideModel.";
        LOG(LEV_ERROR, "GenRepoFac") << err.str();
        throw CycException(err.str());
        break;
    }
    if(kg_to_ext > 0 ) {
      absorb(mat_rsrc_ptr((*daughter)->extract(comp_to_ext, kg_to_ext)));
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<CompMapPtr, double> MixedCellNuclide::inner_neumann(NuclideModelPtr daughter){
  IsoConcMap conc_map;
  ConcGradMap grad_map;
  pair<CompMapPtr, double> comp_pair;
  //flux area perpendicular to flow, timeps porosit, times D.
  double int_factor =2*porosity()*(daughter->geom()->length())*(daughter->geom()->outer_radius());;
  grad_map = daughter->neumann_bc(dirichlet_bc(), geom()->radial_midpoint());
  conc_map = MatTools::scaleConcMap(grad_map, tot_deg()*int_factor);
  IsoConcMap disp_map;
  int iso;
  IsoConcMap::iterator it;
  for(it=conc_map.begin(); it!=conc_map.end(); ++it) {
    if((*it).second >= 0.0){
      iso=(*it).first;
      disp_map[iso] = conc_map[iso]*mat_table_->D(int(iso/1000.));
    }
  }
  comp_pair = MatTools::conc_to_comp_map(disp_map, 1);
  return comp_pair; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<CompMapPtr, double> MixedCellNuclide::inner_dirichlet(NuclideModelPtr daughter){
  IsoConcMap conc_map;
  pair<CompMapPtr, double> comp_pair;
  //flux area perpendicular to flow, times porosity, times v.
  double int_factor =2*v()*porosity()*(daughter->geom()->length())*(daughter->geom()->outer_radius());;
  conc_map = MatTools::scaleConcMap(daughter->dirichlet_bc(), tot_deg()*int_factor);
  IsoConcMap::iterator it;
  for(it=conc_map.begin(); it!=conc_map.end(); ++it) {
    if((*it).second < 0.0){
      (*it).second = 0.0;
    }
  }
  comp_pair = MatTools::conc_to_comp_map(conc_map, 1);
  return comp_pair; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<CompMapPtr, double> MixedCellNuclide::inner_cauchy(NuclideModelPtr daughter){
  pair<CompMapPtr, double> to_ret;
  pair<CompMapPtr, double> neumann = inner_neumann(daughter);
  pair<CompMapPtr, double> dirichlet = inner_dirichlet(daughter);
  double n_kg = neumann.second;
  double d_kg = dirichlet.second;
  IsoVector n_vec = IsoVector(neumann.first);
  IsoVector d_vec = IsoVector(dirichlet.first);
  n_vec.mix(d_vec, n_kg/d_kg);
  return make_pair(n_vec.comp(),n_kg+d_kg);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::sorb(int the_time, int iso, double mass){
  double kd=0;
  if(kd_limited()){
    kd = mat_table_->K_d(MatTools::isoToElem(iso));
  }
  return SolLim::m_ff(mass, kd, V_s(), V_f(), tot_deg());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::precipitate(int the_time, int iso, double m_ff){
  double m_aff = m_ff;
  if(sol_limited()){
    double s = mat_table_->S(MatTools::isoToElem(iso));
    m_aff = SolLim::m_aff(m_ff, V_ff(), s);
    assert(m_ff >= m_aff);
  }
  return m_aff; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::V_f(){
  return MatTools::V_f(V_T(),porosity());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::V_s(){
  return MatTools::V_s(V_T(),porosity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::V_ff(){
  return MatTools::V_ff(V_T(),porosity(), tot_deg());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::V_T(){
  return geom_->volume();
}


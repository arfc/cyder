/*! \file MatTools.cpp
    \brief Implements the MatTools class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <deque>
#include <time.h>
#include <assert.h>


#include "CycException.h"
#include "CycLimits.h"
#include "MatTools.h"
#include "Material.h"
#include "Logger.h"
#include "Timer.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> MatTools::sum_mats(deque<mat_rsrc_ptr> mats){
  IsoVector vec;
  CompMapPtr sum_comp = CompMapPtr(new CompMap(MASS));
  double tot = 0;
  double kg = 0;

  if( !mats.empty() ){ 
    CompMapPtr comp_to_add;
    deque<mat_rsrc_ptr>::iterator mat;
    int iso;
    CompMap::const_iterator comp;

    for(mat = mats.begin(); mat != mats.end(); ++mat){ 
      kg = (*mat)->mass(MassUnit(KG));
      tot += kg;
      comp_to_add = (*mat)->isoVector().comp();
      comp_to_add->massify();
      for(comp = (*comp_to_add).begin(); comp != (*comp_to_add).end(); ++comp) {
        iso = comp->first;
        if(sum_comp->count(iso)!=0) {
          (*sum_comp)[iso] += (comp->second)*kg;
        } else { 
          (*sum_comp)[iso] = (comp->second)*kg;
        }
      }
    }
  } else {
    (*sum_comp)[92235] = 0;
  }
  vec = IsoVector(sum_comp);
  return make_pair(vec, tot);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
mat_rsrc_ptr MatTools::extract(const CompMapPtr comp_to_rem, double kg_to_rem, deque<mat_rsrc_ptr>& mat_list, double threshold){
  comp_to_rem->normalize();
  mat_rsrc_ptr left_over = mat_rsrc_ptr(new Material(comp_to_rem));
  left_over->setQuantity(0);
  while(!mat_list.empty()) { 
    left_over->absorb(mat_list.back());
    mat_list.pop_back();
  }
  mat_rsrc_ptr to_ret = left_over->extract(comp_to_rem, kg_to_rem, KG, threshold);
  if( left_over->mass(KG) > threshold) {  
    mat_list.push_back(left_over);
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MatTools::comp_to_conc_map(CompMapPtr comp, double mass, double vol){
  MatTools::validate_finite_pos(vol);
  MatTools::validate_finite_pos(mass);
  comp->massify();

  IsoConcMap to_ret;
  if( vol==0 ) {
    to_ret = zeroConcMap();
  } else {
    int iso;
    double m_iso;
    CompMap::const_iterator it;
    it=(*comp).begin();
    while(it!= (*comp).end() ){
      iso = (*it).first;
      m_iso=((*it).second)*mass;
      to_ret.insert(make_pair(iso, m_iso/vol));
      ++it;
    } 
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MatTools::zeroConcMap(){
  IsoConcMap to_ret;
  to_ret[92235] = 0;
  return to_ret;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<CompMapPtr, double> MatTools::conc_to_comp_map(IsoConcMap conc, double vol){
  MatTools::validate_finite_pos(vol);

  CompMapPtr comp = CompMapPtr(new CompMap(MASS));
  double mass(0);
  int iso;
  double c_iso;
  double m_iso;
  CompMap::const_iterator it;
  it=conc.begin();
  while(it!= conc.end() ){
    iso = (*it).first;
    c_iso=((*it).second);
    m_iso = c_iso*vol;
    (*comp)[iso] = m_iso;
    mass+=m_iso;
    ++it;
  } 
  (*comp).normalize();
  pair<CompMapPtr, double> to_ret = make_pair(CompMapPtr(comp), mass);
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MatTools::V_f(double V_T, double theta){
  validate_percent(theta);
  validate_finite_pos(V_T);
  return theta*V_T;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MatTools::V_ff(double V_T, double theta, double d){
  validate_percent(d);
  return d*V_f(V_T, theta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MatTools::V_mf(double V_T, double theta, double d){
  return (V_f(V_T,theta) - V_ff(V_T, theta, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MatTools::V_s(double V_T, double theta){
  return (V_T - V_f(V_T, theta));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MatTools::V_ds(double V_T, double theta, double d){
  validate_percent(d);
  return d*V_s(V_T, theta);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MatTools::V_ms(double V_T, double theta, double d){
  return (V_s(V_T, theta) - V_ds(V_T, theta, d));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MatTools::validate_percent(double per){
  if( per <= 1 && per >= 0 ){
    return;
  } else if ( per < 0) {
    throw CycRangeException("The value is not a valid percent. It is less than zero.");
  } else if ( per > 1) {
    throw CycRangeException("The value is not a valid percent. It is greater than one.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MatTools::validate_finite_pos(double pos){
  if ( pos >= numeric_limits<double>::infinity() ) {
    throw CycRangeException("The value is not positive and finite. It is infinite.");
  } 
  validate_pos(pos);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MatTools::validate_pos(double pos){
  if ( pos < 0) {
    std::stringstream ss;
    ss << "The value " 
       << pos
       << " is not positive and finite. It is less than zero." ;;
    throw CycRangeException(ss.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MatTools::validate_nonzero(double nonzero){
  if ( nonzero == 0 )
    throw CycRangeException("The value is zero.");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MatTools::scaleConcMap(IsoConcMap C_0, double scalar){
  MatTools::validate_finite_pos(scalar);
  double orig;
  IsoConcMap::const_iterator it;
  for(it = C_0.begin(); it != C_0.end(); ++it) { 
    orig = C_0[(*it).first];
    C_0[(*it).first] = orig*scalar;
  }
  return C_0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MatTools::addConcMaps(IsoConcMap orig, IsoConcMap to_add){
  IsoConcMap to_ret;
  IsoConcMap::const_iterator it;
  for(it = orig.begin(); it != orig.end(); ++it) {
    Iso iso=(*it).first;
    if(to_add.find(iso) != to_add.end()) {
      to_ret[iso] = (*it).second + to_add[iso];
    } else {
      to_ret[iso] = (*it).second;
    }
  }
  for(it = to_add.begin(); it != to_add.end(); ++it) {
    Iso iso=(*it).first;
    if(orig.find(iso) == orig.end()) {
      to_ret[iso] = (*it).second;
    }
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<CompMapPtr, double> MatTools::subtractCompMaps(pair<CompMapPtr, double> orig_pair, pair<CompMapPtr, double> to_subtract_pair){
  CompMapPtr orig = CompMapPtr(orig_pair.first);
  double o_kg = orig_pair.second;
  CompMapPtr to_subtract = CompMapPtr(to_subtract_pair.first);
  double s_kg = to_subtract_pair.second;
  CompMapPtr to_ret = CompMapPtr(new CompMap(MASS));
  double to_ret_kg = 0;

  CompMap::const_iterator it;
  for(it = (*orig).begin(); it != (*orig).end(); ++it) {
    Iso iso=(*it).first;
    if(to_subtract->map().find(iso) != to_subtract->map().end()) {
      (*to_ret)[iso] = (*it).second*o_kg - (*to_subtract)[iso]*s_kg;
    } else {
      (*to_ret)[iso] = (*it).second*o_kg;
    }
    to_ret_kg += (*to_ret)[iso];
  }
  for(it = (*to_subtract).begin(); it != (*to_subtract).end(); ++it) {
    Iso iso=(*it).first;
    if(orig->map().find(iso) == orig->map().end()) {
      std::stringstream msg_ss;
      msg_ss << "Cannot subtract a superset of isotopes from a subset";
      LOG(LEV_ERROR, "GRDRNuc") <<msg_ss.str();;
      throw CycNegativeValueException(msg_ss.str());
    }
  }
  return make_pair(CompMapPtr(to_ret), to_ret_kg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
int MatTools::isoToElem(int iso) { 
  int N = iso % 1000;
  return (iso-N)/1000;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<double> MatTools::linspace(double a, double b, int n) {
  vector<double> array(0);  // optional preallocation
  double step = (b-a)/(n-1);
  while(a <= b) {
    array.push_back(a);
    a += step;           // could recode to better handle rounding errors
  }
  return array;
}



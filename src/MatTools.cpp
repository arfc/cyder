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
  map<Iso, vector<double> > to_sum;
  vector<double> tot_vec;

  if( !mats.empty() ){ 
    CompMapPtr comp_to_add;
    deque<mat_rsrc_ptr>::iterator mat;
    int iso;
    CompMap::const_iterator comp;
    for(mat = mats.begin(); mat != mats.end(); ++mat){ 
      comp_to_add = (*mat)->unnormalizeComp(MASS, KG);
      for(comp = (*comp_to_add).begin(); comp != (*comp_to_add).end(); ++comp) {
        iso = comp->first;
        if(to_sum.find(iso)==to_sum.end()) {
          to_sum.insert(make_pair(iso, vector<double>()));
        }
        to_sum[iso].push_back(comp->second);
      }
    }
    map<Iso, vector<double> >::const_iterator it; 
    for(it=to_sum.begin(); it!=to_sum.end(); ++it) { 
      iso = (*it).first;
      (*sum_comp)[iso] = KahanSum((*it).second);
      tot_vec.push_back((*sum_comp)[iso]);
    }
    tot = KahanSum(tot_vec);
  }
  vec = IsoVector(sum_comp);
  return make_pair(vec, tot);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MatTools::KahanSum(vector<double> input){
  // http://en.wikipedia.org/wiki/Kahan_summation_algorithm
  double y, t;
  double sum = 0.0;
  //A running compensation for lost low-order bits.
  double c = 0.0; 
  for(vector<double>::iterator i = input.begin(); i!=input.end(); ++i){
    y = *i - c;
    //So far, so good: c is zero.
    t = sum + y;
    //Alas, sum is big, y small, so low-order digits of y are lost.
    c = (t - sum) - y;
    //(t - sum) recovers the high-order part of y; subtracting y recovers -(low part of y)
    sum = t;
    //Algebraically, c should always be zero. Beware eagerly optimising compilers!
    //Next time around, the lost low part will be added to y in a fresh attempt.
  }
  return sum;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool MatTools::matSortCriterion(const mat_rsrc_ptr mat1, const mat_rsrc_ptr mat2){
  return mat1->mass(MassUnit(KG))<mat2->mass(MassUnit(KG));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
mat_rsrc_ptr MatTools::extract(const CompMapPtr comp_to_rem, double kg_to_rem, 
    deque<mat_rsrc_ptr>& mat_list, double threshold){
  // sort mats from smallest to largest.
  sort(mat_list.begin(), mat_list.end(), MatTools::matSortCriterion);
  mat_rsrc_ptr left_over = mat_rsrc_ptr(new Material());
  left_over->setQuantity(0);
  // absorb them together.
  while(!mat_list.empty()) { 
    left_over->absorb(mat_list.front());
    mat_list.pop_front();
  }
  mat_rsrc_ptr to_ret;
  if( left_over->mass(KG) >= threshold ) {
    to_ret = left_over->extract(comp_to_rem, kg_to_rem, KG, threshold);
    mat_list.push_back(left_over);
  } else { 
    to_ret = mat_rsrc_ptr(left_over);
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
  vector<double> masses;
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
    masses.push_back(m_iso);
    ++it;
  } 
  mass = MatTools::KahanSum(masses);
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
  map<Iso, vector<double> > add_map;
  IsoConcMap to_ret;
  IsoConcMap::const_iterator it;
  for(it = orig.begin(); it != orig.end(); ++it) {
    Iso iso=(*it).first;
    add_map[iso].push_back((*it).second);
    if(to_add.find(iso) != to_add.end()) {
      add_map[iso].push_back(to_add[iso]);
    } 
  }
  for(it = to_add.begin(); it != to_add.end(); ++it) {
    Iso iso=(*it).first;
    if(orig.find(iso) == orig.end()) {
      add_map[iso].push_back((*it).second);
    }
  }

  map<Iso, vector<double> >::const_iterator term;
  for( term=add_map.begin(); term!=add_map.end(); ++term){
    Iso iso=(*term).first;
    to_ret[iso] = MatTools::KahanSum((*term).second);
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
int MatTools::isoToElem(int iso) { 
  int N = iso % 1000;
  return (iso-N)/1000;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<double> MatTools::linspace(double a, double b, int n) {
  vector<double> vec;  
  double step = (b-a)/(n-1);
  for( int i=0; i<n; ++i){
    vec.push_back(a+i*step);
  }
  return vec;
}



// MatDataTable class

#include <iostream>
#include <stdlib.h>
#include <sstream>

#include "MatDataTable.h"
#include "SqliteDb.h"

#include "Env.h"
#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTable::MatDataTable() :
  mat_(""),
  elem_len_(0)
{
  initialized_= false;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTable::MatDataTable(string mat, vector<element_t> elem_vec, map<Elem, 
    int> elem_index, double ref_disp, double ref_kd, double 
    ref_sol) :
  mat_(mat),
  elem_vec_(elem_vec),
  elem_index_(elem_index),
  ref_disp_(ref_disp),
  ref_kd_(ref_kd),
  ref_sol_(ref_sol)
{
  elem_len_= elem_vec_.size();
  initialized_=true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTable::~MatDataTable() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::K_d(Elem ent){
  check_validity(ent);
  return data(ent, KD);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::S(Elem ent){
  check_validity(ent);
  return data(ent, SOL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::D(Elem ent){
  check_validity(ent);
  return data(ent, DISP);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MatDataTable::check_validity(Elem ent) { 
  map<Elem, int>::iterator it;
  it=elem_index_.find(ent);
  if (it==elem_index_.end()){
    stringstream err;
    err << "Element " << ent << " not valid";
    throw CycException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::data(Elem ent, ChemDataType data) {
  double to_ret;
  switch( data ){
    case DISP :
      to_ret = ref(DISP)*rel(ent, DISP);
      break;
    case KD :
      to_ret = ref(KD)*rel(ent, KD);
      break;
    case SOL :
      to_ret = ref(SOL)*rel(ent, SOL);
      break;
    default : 
      throw CycException("The ChemDataType provided is not yet supported.");
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::rel(Elem ent, ChemDataType data) {
  double to_ret;
  int h = elem_index_[1];
  int ind = elem_index_[ent];
  switch( data ){
    case DISP : 
      to_ret = elem_vec_[ind].D/elem_vec_[h].D;
      break;
    case KD :
      to_ret = elem_vec_[ind].K_d/elem_vec_[h].K_d;
      break;
    case SOL :
      to_ret = elem_vec_[ind].S/elem_vec_[h].S;
      break;
    default : 
      throw CycException("The ChemDataType provided is not yet supported.");
  }
  return to_ret;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::ref(ChemDataType data){
  double to_ret;
  int h = elem_index_[1];
  switch( data ){
    case DISP : 
      (initialized_) ?  (to_ret=ref_disp_) : (to_ret=elem_vec_[h].D) ;
      break;
    case KD :
      (initialized_) ?  (to_ret=ref_kd_) : (to_ret=elem_vec_[h].K_d) ;
      break;
    case SOL :
      (initialized_) ?  (to_ret=ref_sol_) : (to_ret=elem_vec_[h].S) ;
      break;
    default : 
      throw CycException("The ChemDataType provided is not yet supported.");
  }
  return to_ret;
}

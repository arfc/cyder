// STCDataTable class

#include <iostream>
#include <stdlib.h>
#include <sstream>

#include "STCDataTable.h"
#include "SqliteDb.h"

#include "Env.h"
#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTable::STCDataTable() :
  mat_(""),
  elem_len_(0) {
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTable::STCDataTable(string mat, vector<element_t> elem_vec, map<Elem, int> elem_index) :
  mat_(mat),
  elem_vec_(elem_vec),
  elem_index_(elem_index)
{
  elem_len_= elem_vec_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTable::~STCDataTable() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double STCDataTable::K_d(Elem ent){
  check_validity(ent);
  return elem_vec_[ent].K_d;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double STCDataTable::S(Elem ent){
  check_validity(ent);
  return elem_vec_[ent].S;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double STCDataTable::D(Elem ent){
  check_validity(ent);
  return elem_vec_[ent].D;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void STCDataTable::check_validity(Elem ent) { 
  map<Elem, int>::iterator it;
  it=elem_index_.find(ent);
  if (it==elem_index_.end()){
    stringstream err;
    err << "Element " << ent << " not valid";
    throw CycException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double STCDataTable::data(Elem ent, ChemDataType data) {
  double to_ret;
  switch( data ){
    case DISP :
      to_ret = D(ent);
      break;
    case KD :
      to_ret = K_d(ent);
      break;
    case SOL :
      to_ret = S(ent);
      break;
    default : 
      throw CycException("The ChemDataType provided is not yet supported.");
  }
  return to_ret;
}


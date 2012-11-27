// MatDataTable class

#include <iostream>
#include <stdlib.h>

#include "MatDataTable.h"
#include "SqliteDb.h"

#include "Env.h"
#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTable::MatDataTable() :
  mat_(""),
  elem_len_(0) {
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTable::MatDataTable(string mat, vector<element_t> elem_vec, map<Elem, int> elem_index) :
  mat_(mat),
  elem_vec_(elem_vec),
  elem_index_(elem_index)
{
  elem_len_= elem_vec_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTable::~MatDataTable() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::K_d(string mat, Elem ent){
  check_validity(ent);
  return elem_vec[ent].K_d;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::S(string mat, Elem ent){
  check_validity(ent);
  return elem_vec[ent].S;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::D(string mat, Elem ent){
  check_validity(ent);
  return elem_vec[ent].D;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MatDataTable::check_validity(Elem ent) { 
  map<Elem, int>::iterator it;
  it=elem_vec_.find(ent);
  if (it==elem_vec_.end()){
    throw CycException("Element "+ent+" not valid");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::data(Elem ent, ChemDataType data) {
  switch data {
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<Elem, double> MatDataTable::data(string mat, string data) {
  map<Elem, double> to_ret;

  return to_ret;
}




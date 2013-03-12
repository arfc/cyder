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
  name_(""),
  n_isos_(0) {

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTable::STCDataTable(string name, vector<stc_t> stc_vec, map<Iso, int> iso_index) :
  name_(name),
  stc_vec_(stc_vec),
  iso_index_(iso_index)
{
  n_isos_= stc_vec_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTable::~STCDataTable() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double STCDataTable::stc(Iso tope){
  return stc_vec_[isoToInd(tope)].stc;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int STCDataTable::isoToInd(Iso tope) {
  check_validity(tope)
  map<Iso, int>::iterator it=iso_index_.find(tope);
  return(*it).second;
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


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
  name_("")
{

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTable::STCDataTable(string name, boost::multi_array<double, 2> stc_array, map<Iso, int> 
    iso_index, map<int, int> time_index) :
  name_(name),
  stc_array_(stc_array),
  iso_index_(iso_index)
  time_index_(time_index)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTable::~STCDataTable() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double STCDataTable::stc(Iso tope, int the_time){
  return stc_array_[isoToInd(tope)][timeToInd(the_time)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int STCDataTable::timeToInd(int the_time) {
  check_validity(the_time, time_index_);
  map<int, int>::iterator it=time_index_.find(the_time);
  return(*it).second;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int STCDataTable::isoToInd(Iso tope) {
  check_validity(tope, iso_index_);
  map<Iso, int>::iterator it=iso_index_.find(tope);
  return(*it).second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void STCDataTable::check_validity(int val, map<int, int> index) { 
  map<int, int>::iterator it;
  it=index.find(val);
  if (it==index.end()){
    stringstream err;
    err << "Value " << val << " not valid in the index.";
    throw CycException(err.str());
  }
}


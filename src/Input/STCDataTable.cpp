// STCDataTable class

#include <iostream>
#include <stdlib.h>
#include <sstream>

#include "Logger.h"
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
  iso_index_(iso_index),
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
int STCDataTable::indToVal(int ind, map<int,int> val_index){
  try{
    int to_ret =val_index.at(ind);
    return to_ret;
  } catch (const out_of_range& oor){
    stringstream msg_ss;
    msg_ss << oor.what();
    msg_ss << "The index ";
    msg_ss << ind;
    msg_ss << " is outside of the range of the stc table map.";
    LOG(LEV_ERROR, "CydSTC") << msg_ss.str();
    throw CycRangeException(msg_ss.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int STCDataTable::indToIso(int ind) {
  int to_ret = indToVal(ind, iso_index_);
  return to_ret;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int STCDataTable::indToTime(int ind) {
  int to_ret = indToVal(ind, time_index_);
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int STCDataTable::timeToInd(int the_time) {
  checkValidity(the_time, time_index_);
  map<int, int>::iterator it=time_index_.find(the_time);
  return(*it).second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int STCDataTable::isoToInd(Iso tope) {
  checkValidity(tope, iso_index_);
  map<Iso, int>::iterator it=iso_index_.find(tope);
  return(*it).second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void STCDataTable::checkValidity(int val, map<int, int> val_index) { 
  map<int, int>::iterator it;
  it=val_index.find(val);
  if (it==val_index.end()){
    stringstream err;
    err << "Value " << val << " not valid in the index.";
    throw CycException(err.str());
  }
}


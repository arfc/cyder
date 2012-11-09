// DiffCoeffTable class

#include <iostream>
#include <stdlib.h>

#include "DiffCoeffTable.h"

#include "Env.h"
#include "Database.h"
#include "CycException.h"

using namespace std;

DiffCoeffTable* DiffCoeffTable::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DiffCoeffTable* DiffCoeffTable::Instance() {
  // If we haven't created a DiffCoeffTable yet, create it, and then and return it
  // either way.
  if (0 == instance_) {
    instance_ = new DiffCoeffTable();
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DiffCoeffTable::DiffCoeffTable():
  file_name_("diffcoeff.sqlite"),
{
  initializeSQL();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void initializeTableMembers(QueryEngine* qe){
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DiffCoeffTable::~DiffCoeffTable() {
  //Should close the 'diffcoeff.sqlite' file
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double DiffCoeffTable::diff(int index) {
  double toRet = elem_vec_[elt_matrix_index_[tope]].diff;
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DiffCoeffTable::initializeSQL() {
  Database* db = open_db()
  query_result mnums = db->query("SELECT matrix FROM diffcoeffs");
  query_result znums = db->query("SELECT Z FROM diffcoeffs");
  query_result dnums = db->query("SELECT diff FROM diffcoeffs");
  
  for (int i = 0; i < znums.size(); i++){
    string mStr = mnums.at(i).at(0);
    string zStr = znums.at(i).at(0);
    string dStr = dnums.at(i).at(0);
    int m = atoi( mStr.c_str() );
    int z = atoi( zStr.c_str() );
    double diff = atof( dStr.c_str() );
    elem_t elt = {z, m, diff};
    elem_vec_.push_back(elt);
    int e_m_index = z*1000 + m; 
    elt_matrix_index_.insert(make_pair(e_m_index, i));
  }
  // set the total number of elements
  elem_len_ = elem_vec_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string file_path() { 
  return Env::getBuildPath() + "/share";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string file_name() {
  return file_name_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Database* open_db() { 
  Database *db = new Database( file_name(), file_path());
  db->open();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double DiffCoeffTable::diff(int elt, string matrix) {
  switch case
}

// MatDataTable class

#include <iostream>
#include <stdlib.h>

#include "MatDataTable.h"

#include "Env.h"
#include "Database.h"
#include "CycException.h"

using namespace std;

MatDataTable* MatDataTable::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTable* MatDataTable::Instance() {
  // If we haven't created a MatDataTable yet, create it, and then and return it
  // either way.
  if (0 == instance_) {
    instance_ = new MatDataTable();
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTable::MatDataTable():
  file_name_("repo_data.sqlite"),
{
  initializeSQL();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void initializeTableMembers(QueryEngine* qe){
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTable::~MatDataTable() {
  //Should close the 'diffcoeff.sqlite' file
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MatDataTable::diff(int index) {
  double toRet = elem_vec_[elt_matrix_index_[tope]].diff;
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MatDataTable::initializeSQL() {
  Database* db = open_db()
  query_result mnums = db->query("SELECT matrix FROM data_name");
  query_result znums = db->query("SELECT Z FROM data_name");
  query_result dnums = db->query("SELECT diff FROM data_name");
  
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
double MatDataTable::diff(int elt, string matrix) {
}

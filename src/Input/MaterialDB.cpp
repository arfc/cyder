// MaterialDB class

#include <iostream>
#include <stdlib.h>

#include "MaterialDB.h"

#include "Env.h"
#include "CycException.h"

using namespace std;

MaterialDB* MaterialDB::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MaterialDB* MaterialDB::Instance() {
  // If we haven't created a MaterialDB yet, create it. 
  // Return it either way
  if (0 == instance_) {
    instance_ = new MaterialDB();
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MaterialDB::MaterialDB() {
  file_path_ = Env::getInstallPath() + "/share/mat_data.sqlite";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MaterialDB::~MaterialDB() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialDB::K_d(string mat, Elem ent){
  return table(mat)->K_d(ent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialDB::S(string mat, Elem ent){
  return table(mat)->S(ent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialDB::D(string mat, Elem ent){
  return table(mat)->D(ent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTablePtr MaterialDB::table(string mat) {
  MatDataTablePtr to_ret;
  if(initialized(mat) ){
    to_ret = (*tables_.find(mat)).second;
  } else {
    to_ret = initializeFromSQL(mat);
    tables_.insert(make_pair(mat,to_ret));
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool MaterialDB::initialized(string mat){
  map<string,MatDataTablePtr>::iterator it;
  it=tables_.find(mat);
  return it!=tables_.end()? true : false; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTablePtr MaterialDB::initializeFromSQL(string mat) {
  SqliteDb* db = new SqliteDb(file_path_);

  std::vector<StrList> znums = db->query("SELECT elem FROM "+mat);
  std::vector<StrList> dnums = db->query("SELECT d FROM "+mat);
  std::vector<StrList> knums = db->query("SELECT k_d FROM "+mat);
  std::vector<StrList> snums = db->query("SELECT s FROM "+mat);
 
  vector<element_t> elem_vec;
  map<Elem, int> elem_index;
  for (int i = 0; i < znums.size(); i++){
    // // obtain the database row and declare the appropriate members
    string zStr = znums.at(i).at(0);
    string dStr = dnums.at(i).at(0);
    string kStr = knums.at(i).at(0);
    string sStr = snums.at(i).at(0);
    Elem z = atoi( zStr.c_str() );
    double d = atof( dStr.c_str() );
    double k = atof( kStr.c_str() );
    double s = atof( sStr.c_str() );
    // create a element member and add it to the element vector
    element_t e = {z, d, k, s};
    elem_vec.push_back(e);
    // log it accordingly
    elem_index.insert(make_pair(z, i));
  }
  MatDataTablePtr to_ret = MatDataTablePtr(new MatDataTable(mat, elem_vec, elem_index)); 
  delete db;
  return to_ret;
}



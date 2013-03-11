// STCDB class

#include <iostream>
#include <stdlib.h>

#include "STCDB.h"

#include "Env.h"
#include "CycException.h"

using namespace std;

STCDB* STCDB::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDB* STCDB::Instance() {
  // If we haven't created a STCDB yet, create it. 
  // Return it either way
  if (0 == instance_) {
    instance_ = new STCDB();
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDB::STCDB() :
  file_path_(Env::getInstallPath() + "/share/stc_data.sqlite") {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDB::~STCDB() {
  delete instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double STCDB::K_d(string mat, Elem ent){
  return table(mat)->K_d(ent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double STCDB::S(string mat, Elem ent){
  return table(mat)->S(ent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double STCDB::D(string mat, Elem ent){
  return table(mat)->D(ent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTablePtr STCDB::table(string mat) {
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
bool STCDB::initialized(string mat){
  map<string,MatDataTablePtr>::iterator it;
  it=tables_.find(mat);
  return it!=tables_.end()? true : false; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTablePtr STCDB::mat_name(mat_t mat){
  std::string mat_name = 
    "a"+boost::lexical_cast<string>(mat.alpha_th)+
    "k"+boost::lexical_cast<string>(mat.k_th)+
    "s"+boost::lexical_cast<string>(mat.spacing)+
    "r"+boost::lexical_cast<string>(mat.r_calc); 
  return mat_name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTablePtr STCDB::initializeFromSQL(mat_t mat);
  SqliteDb* db = new SqliteDb(file_path_);
  std::string mat_name = mat_name(mat);
  vector<stc_t> stc_vec = stc_vec(db, mat);
  map<Iso, int> iso_index = iso_index(db, mat);
  STCDataTablePtr to_ret = STCDataTablePtr(new STCDataTable(mat_name, stc_vec, iso_index));
  delete db;
  return to_ret;
}

MatDataTablePtr STCDB::

  std::vector<StrList> znums = db->query("SELECT iso FROM "+table);
  std::vector<StrList> dnums = db->query("SELECT r_calc FROM "+mat);
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



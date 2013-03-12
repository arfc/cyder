// STCDB class

#include <boost/lexical_cast.hpp>
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
double STCDB::stc(mat_t mat, Iso tope){
  return table(mat)->stc(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTablePtr STCDB::table(mat_t mat) {
  STCDataTablePtr to_ret;
  if(initialized(mat_name(mat)) ){
    to_ret = (*tables_.find(mat_name(mat))).second;
  } else {
    to_ret = initializeFromSQL(mat);
    tables_.insert(make_pair(mat_name(mat),to_ret));
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool STCDB::initialized(string mat){
  map<string,STCDataTablePtr>::iterator it;
  it=tables_.find(mat);
  return it!=tables_.end()? true : false; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string STCDB::mat_name(mat_t mat){
  std::string mat_name = 
    "a"+boost::lexical_cast<string>(mat.alpha_th)+
    "k"+boost::lexical_cast<string>(mat.k_th)+
    "s"+boost::lexical_cast<string>(mat.spacing)+
    "r"+boost::lexical_cast<string>(mat.r_calc); 
  return mat_name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTablePtr STCDB::initializeFromSQL(mat_t mat){
  SqliteDb* db = new SqliteDb(file_path_);
  vector<stc_t> vec = stc_vec(db, mat);
  map<Iso, int> index = iso_index(db, mat);
  STCDataTablePtr to_ret = STCDataTablePtr(new STCDataTable(mat_name(mat), vec, index));
  delete db;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string STCDB::whereClause(mat_t mat){
  string where_clause = "WHERE alpha_th="+ 
    boost::lexical_cast<string>( mat.alpha_th ) + 
    " AND k_th="    + boost::lexical_cast<string>( mat.k_th ) +
    " AND spacing=" + boost::lexical_cast<string>( mat.spacing ) +
    " AND r_calc="  + boost::lexical_cast<string>( mat.r_calc );
  return where_clause;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<Iso, int> STCDB::iso_index(SqliteDb* db, mat_t mat){

  std::vector<StrList> inums = db->query("SELECT iso FROM STCData " + whereClause(mat));
 
  map<Iso, int> iso_index;
  for (int i = 0; i < inums.size(); i++){
    // // obtain the database row and declare the appropriate members
    string iStr = inums.at(i).at(0);
    Iso tope = atoi( iStr.c_str() );
    // log it accordingly
    iso_index.insert(make_pair(tope, i));
  }
  return iso_index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<stc_t> STCDB::stc_vec(SqliteDb* db, mat_t mat){
  std::vector<StrList> inums = db->query("SELECT iso FROM STCData " + whereClause(mat));
  std::vector<StrList> snums = db->query("SELECT stc FROM STCData " + whereClause(mat));
 
  vector<stc_t> stc_vec;
  for (int i = 0; i < inums.size(); i++){
    // // obtain the database row and declare the appropriate members
    string iStr = inums.at(i).at(0);
    string sStr = snums.at(i).at(0);
    Iso tope = atoi( iStr.c_str() );
    double s = atof( sStr.c_str() );
    // create a element member and add it to the element vector
    stc_t v = {tope, s}; 
    stc_vec.push_back(v);
  }
  return stc_vec;
}



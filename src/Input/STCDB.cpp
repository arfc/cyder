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
double STCDB::stc(mat_t mat, Iso tope, int the_time){
  return table(mat)->stc(tope, the_time);
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
  boost::multi_array<double, 2> arr = stc_array(db, mat);
  STCDataTablePtr to_ret = STCDataTablePtr(new STCDataTable(mat_name(mat), arr, 
        iso_index(db, mat), time_index(db, mat)));
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

  std::vector<StrList> inums = db->query("SELECT iso FROM STCData " + 
      whereClause(mat));
 
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
map<Iso, int> STCDB::time_index(SqliteDb* db, mat_t mat){

  std::vector<StrList> inums = db->query("SELECT time FROM STCData " + 
      whereClause(mat));
 
  map<int, int> time_index;
  for (int t = 0; t < tnums.size(); t++){
    // // obtain the database row and declare the appropriate members
    strtng tStr = tnums.at(t).at(0);
    int the_time = atoi( tStr.c_str() );
    // log it accordingly
    time_index.insert(make_pair(the_time, t));
  }
  return time_index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<stc_t> STCDB::n_timesteps(SqliteDb* db, mat_t mat){
  std::vector<StrList> tnums = db->query("SELECT DISTINCT time FROM STCData " + 
      whereClause(mat));
  int n_timesteps = tnums.size();
  return n_timesteps;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<stc_t> STCDB::n_isos(SqliteDb* db, mat_t mat){
  std::vector<StrList> inums = db->query("SELECT DISTINCT iso FROM STCData " + 
      whereClause(mat));
  int n_isos = inums.size();
  return n_isos;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
boost::multi_array<double, 2> STCDB::stc_arrae(SqliteDb* db, mat_t mat){
  std::vector<StrList> inums = db->query("SELECT iso FROM STCData " + 
      whereClause(mat));
  std::vector<StrList> snums = db->query("SELECT stc FROM STCData " + 
      whereClause(mat));
  std::vector<StrList> tnums = db->query("SELECT time FROM STCData " + 
      whereClause(mat));
  
  boost::multi_array<double, 2> stc_array(boost::extents(n_isos(db, mat), 
        n_timesteps(db, mat)));

  vector<stc_t> stc_vec;
  for (int i = 0; i < inums.size(); i++){
    // // obtain the database row and declare the appropriate members
    string iStr = inums.at(i).at(0);
    string sStr = snums.at(i).at(0);
    string tStr = tnums.at(i).at(0);
    Iso tope = atoi( iStr.c_str() );
    int the_time = atoi( tStr.c_str() );
    double stc = atof( sStr.c_str() );
    // determine array indcies and add stc to the stc array
    stc_array[iso_index(db, mat)[iso]][time_index(db, mat)[the_time]] = stc;
  }
  return stc_array;
}



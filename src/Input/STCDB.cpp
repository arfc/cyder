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
string STCDB::mat_name(mat_t mat){
  string mat_name = 
    "a"+boost::lexical_cast<string>(mat.alpha_th)+
    "k"+boost::lexical_cast<string>(mat.k_th)+
    "s"+boost::lexical_cast<string>(mat.spacing)+
    "r"+boost::lexical_cast<string>(mat.r_calc); 
  return mat_name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCDataTablePtr STCDB::initializeFromSQL(mat_t mat){
  bool readonly = true;
  SqliteDb* db = new SqliteDb(file_path_, readonly);
  boost::multi_array<double, 2> arr = stc_array(db, mat);
  string stc_table_id = table_id(db, mat);
  STCDataTablePtr to_ret = STCDataTablePtr(new STCDataTable(mat_name(mat), arr, 
        iso_index(db, stc_table_id), time_index(db, stc_table_id)));
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
map<Iso, int> STCDB::iso_index(SqliteDb* db, string table_id){

  vector<StrList> inums = db->query("SELECT DISTINCT iso FROM " + table_id);
  return getIndex(inums);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<int, int> STCDB::time_index(SqliteDb* db, string table_id){
  vector<StrList> tnums = db->query("SELECT DISTINCT time FROM " + table_id);
  return getIndex(tnums);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<int, int> getIndex(vector<StrList> vals){
  map<int, int> to_ret;
  for(int i = 0; i < vals.size(); i++){
    // // obtain the database row and declare the appropriate members
    string vStr = vals.at(i).at(0);
    int the_val = atoi( vStr.c_str() );
    // log it accordingly
    to_ret.insert(make_pair(the_val, i));
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string STCDB::table_id(SqliteDb* db, mat_t mat) {
  vector<StrList> mat_id = db->query("SELECT mat_id FROM STCData " + 
      whereClause(mat));
  
  string stc_table_id = "mat"+boost::lexical_cast<string>( mat_id.at(0).at(0) );
  return stc_table_id;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
boost::multi_array<double, 2> STCDB::stc_array(SqliteDb* db, mat_t mat){

  string stc_table_id = table_id(db, mat);
  vector<StrList> inums = db->query("SELECT iso FROM " + stc_table_id); 
  vector<StrList> snums = db->query("SELECT stc FROM " + stc_table_id); 
  vector<StrList> tnums = db->query("SELECT time FROM " + stc_table_id); 

  map<int,int> time_map = time_index(db, stc_table_id);
  map<Iso,int> iso_map = iso_index(db, stc_table_id);
  int n_isos = iso_map.size();
  int n_timesteps = time_map.size();

  boost::multi_array<double, 2> to_ret(boost::extents[n_isos][n_timesteps]);

  for (int i = 0; i < inums.size(); i++){
    // // obtain the database row and declare the appropriate members
    string iStr = inums.at(i).at(0);
    string sStr = snums.at(i).at(0);
    string tStr = tnums.at(i).at(0);
    Iso tope = atoi( iStr.c_str() );
    int the_time = atoi( tStr.c_str() );
    double temp_change = atof( sStr.c_str() );
    // determine array indcies and add stc to the stc array
    to_ret[iso_map[tope]][time_map[the_time]] = temp_change;
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<double> STCDB::k_th_range(){
  
  return k_th_range_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<double> STCDB::alpha_th_range(){
  return alpha_th_range_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<double> STCDB::spacing_range(){
  return spacing_range_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<double> STCDB::r_calc_range(){
  return r_calc_range_;
}


// MaterialDB class

#include <iostream>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

#include "MaterialDB.h"

#include "Env.h"
#include "CycException.h"
#include "Logger.h"

using namespace std;

MaterialDB* MaterialDB::instance_ = 0;
int MaterialDB::table_id_ = 0;

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
MaterialDB::MaterialDB() :
  file_path_(Env::getInstallPath() + "/share/mat_data.sqlite") {
    disp_ind_map_[0]=0;
    kd_ind_map_[0]=0;
    sol_ind_map_[0]=0;
    table_id_array_[0][0][0]=0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MaterialDB::~MaterialDB() {
  delete instance_;
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
MatDataTablePtr MaterialDB::table(string mat, double ref_disp, double 
    ref_kd, double ref_sol) {
  MatDataTablePtr to_ret;
  string ID = tableID(mat, ref_disp, ref_kd, ref_sol);

  if(initialized(ID)) {
    to_ret = (*tables_.find(ID)).second;
  } else {
    to_ret = initializeFromSQL(mat, ref_disp, ref_kd, ref_sol);
    tables_.insert(make_pair(ID,to_ret));
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
string MaterialDB::tableID(string mat, double ref_disp, double ref_kd, 
    double ref_sol){
  if(ref_disp == NULL){ref_disp=0;};
  if(ref_kd == NULL){ref_kd=0;};
  if(ref_sol == NULL){ref_sol=0;};
  int disp_ind = ref_ind(ref_disp, DISP);
  int kd_ind = ref_ind(ref_kd, KD);
  int sol_ind = ref_ind(ref_sol, SOL);

  int ref_id;

  try{
    ref_id = table_id_array_.at(disp_ind).at(kd_ind).at(sol_ind); 
  } catch(const out_of_range& oor){
    table_id_array_[disp_ind][kd_ind][sol_ind] = table_id_++;
    ref_id = table_id_array_[disp_ind][kd_ind][sol_ind]; 
  }

  return mat+boost::lexical_cast<string>(ref_id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int MaterialDB::ref_ind(double ref, ChemDataType data){
  int to_ret;
  map<double, int>* ref_map;
  switch (data) {
    case DISP:
      ref_map = &disp_ind_map_;
      break;
    case KD:
      ref_map = &kd_ind_map_;
      break;
    case SOL:
      ref_map = &sol_ind_map_;
      break;
    default:
     string err = "The ChemDataType '"; 
     err += data; 
     err += "' is not supported.";
     LOG(LEV_ERROR,"GRMDB") << err;
     throw CycException(err); 
     break;
  }
 if( (*ref_map).find(ref) != (*ref_map).end() ){
   to_ret=(*ref_map)[ref];
 } else { 
   to_ret = (*ref_map).size();
   (*ref_map).insert(make_pair(ref, to_ret));
 }
 return to_ret;

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool MaterialDB::initialized(string ID){
  map<string,MatDataTablePtr>::iterator it;
  it=tables_.find(ID);
  return it!=tables_.end()? true : false; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTablePtr MaterialDB::initializeFromSQL(string mat, double ref_disp,
    double ref_kd, double ref_sol) {
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
  MatDataTablePtr to_ret = MatDataTablePtr(new MatDataTable(mat, elem_vec, elem_index, 
        ref_disp, ref_kd, ref_sol)); 
  delete db;
  return to_ret;
}



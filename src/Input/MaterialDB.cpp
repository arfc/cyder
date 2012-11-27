// MaterialDB class

#include <iostream>
#include <stdlib.h>

#include "MaterialDB.h"
#include "SqliteDb.h"

#include "Env.h"
#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MaterialDB::MaterialDB() {
  string file_path = Env::getBuildPath() + "/share/mat_data.sqlite";
  SqliteDb* db_ = new SqliteDb(file_path);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MaterialDB::~MaterialDB() {
  delete db_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialDB::K_d(string mat, Elem ent){
  table(mat)->data(ent, "K_d");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialDB::S(string mat, Elem ent){
  table(mat)->data(ent, "S");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialDB::D(string mat, Elem ent){
  table(mat)->data(ent, "D");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialDB::data(string mat, Elem ent, string data) {
  return table(mat)->data(ent, data);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<Elem, double> MaterialDB::data(string mat, string data) {
  return table(mat)->data(data);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool MaterialDB::initialized(string mat){
  map<string,MatDataTablePtr>::iterator it;
  it=tables_.find(mat);
  return it!=tables_.end()? true : false; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTablePtr MaterialDB::table(string mat) {
  MatDataTablePtr to_ret;
  if initialized() {
    to_ret=tables_.find(mat).second;
  } else {
    to_ret = initializeFromSQL(mat);
    tables_.insert(make_pair(mat,to_ret));
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatDataTablePtr MaterialDB::initializeFromSQL(string mat) {
    std::vector<StrList> znums = db->query("SELECT Z FROM "+mat);
    std::vector<StrList> dnums = db->query("SELECT D FROM "+mat);
    std::vector<StrList> knums = db->query("SELECT K_d FROM "+mat);
    std::vector<StrList> snums = db->query("SELECT S FROM "+mat);
   
    vector<element_t> elem_vec;
    map<Elem, int> elem_index;
    for (int i = 0; i < znums.size(); i++){
      // // obtain the database row and declare the appropriate members
      string zStr = znums.at(i).at(0);
      string dStr = mnums.at(i).at(0);
      string kStr = mnums.at(i).at(0);
      string sStr = mnums.at(i).at(0);
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
  MatDataTablePtr table = MatDataTablePtr(new MatDataTable(mat, elem_vec, elem_index)); 
  return table;
}



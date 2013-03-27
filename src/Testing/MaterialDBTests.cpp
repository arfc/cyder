// MaterialDBTests.cpp
#include <gtest/gtest.h>
#include "MaterialDBTests.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, DISABLED_openDB){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, DISABLED_closeDB){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, DISABLED_listAvailableMats){
  // the DB should include one table for each important mat
  // Clay, Granite, Bedrock, Salt, Steel, Copper, Glass, Bentonite, UO2
  //EXPECT_NO_THROW(MDB->list_available_mats());
  //EXPECT_EQ(1,MDB->list_available_mats().count("clay"))
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, DISABLED_get_mat_table){
  // the DB should return a table for any mat in the list of avail mats
 // set<std::string>::iterator it;
 // for(it=MDB->list_available_mats().begin();
 //     it!=MDB->list_available_mats().end(); ++it){
 //   EXPECT_NO_THROW(MDB->table((*it)))
 // }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, DISABLED_listAvailableElems){
  // the DB should include all elements for each mat
  //MatDataTablePtr clay_table = MDB->table("clay");
  //EXPECT_NO_THROW(clay_table->list_available_elems());
  //for(int i=1; i<120; i++){
  //  EXPECT_EQ(1,MDB->list_available_elems().count(i))
 // }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, DISABLED_get_elem_row){
  // from a table object,
  // the DB should return a row for any elem in the list of avail elems
  //MatDataTablePtr clay_table = MDB->table("clay");
  //set<std::int>::iterator it;
  //for(it=clay_table->list_available_elems().begin();
  //    it!=clay_table->list_available_elems().end(); ++it){
  //  EXPECT_NO_THROW(clay_table->data((*it)));
 // }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, DISABLED_availableData){
  // the DB should include K_d, S, D, etc. for each elem

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, DISABLED_get_data_elem){
  // from a row object,
  // the DB should return a data element for any data type in the list of 
  // available data types
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, S){
  // the DB should give appropriate solubility limits for canonical mat/elems, within a range.
  std::vector<Elem>::iterator it;
  std::string matID = MDB->tableID("clay");
  for(it=elem_ids_.begin(); it<elem_ids_.end(); it++){
    EXPECT_NO_THROW(MDB->S(matID, (*it)));
    EXPECT_GT(MDB->S(matID, (*it)),0);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, K_d){
  // the DB should give appropriate K_d values for canonical mat/elems, within a range.
  std::vector<Elem>::iterator it;
  std::string matID = MDB->tableID("clay");
  for(it=elem_ids_.begin(); it<elem_ids_.end(); it++){
    EXPECT_NO_THROW(MDB->K_d(matID, (*it)));
    EXPECT_GT(MDB->K_d(matID, (*it)),0);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, D){
  // the DB should give appropriate D values for canonical mat/elems, within a range.
  std::vector<Elem>::iterator it;
  std::string matID = MDB->tableID("clay");
  for(it=elem_ids_.begin(); it<elem_ids_.end(); it++){
    EXPECT_NO_THROW(MDB->D(matID, (*it)));
    EXPECT_GT(MDB->D(matID, (*it)),0);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, infSolLimits){
  // Some elements can have effectively infinite solubility limits. 
  // Check that this is communicated.
  std::string matID = MDB->tableID("clay");
  EXPECT_NO_THROW(MDB->S(matID, 53));
}



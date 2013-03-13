// STCDBTests.cpp
#include <gtest/gtest.h>
#include "STCDBTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCDBTest, createInstance){
  EXPECT_NO_THROW(STCDB());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCDBTest, initializeFromSQL){
  EXPECT_NO_THROW(STCDB());
  EXPECT_NO_THROW(SDB->initializeFromSQL(salt_struct_));
  SDB->initializeFromSQL(salt_struct_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCDBTest, DISABLED_get_mat_table){
  // the DB should return a table for any mat in the list of avail mats
 // set<std::string>::iterator it;
 // for(it=MDB->list_available_mats().begin();
 //     it!=MDB->list_available_mats().end(); ++it){
 //   EXPECT_NO_THROW(MDB->table((*it)))
 // }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCDBTest, DISABLED_listAvailableElems){
  // the DB should include all elements for each mat
  //MatDataTablePtr clay_table = MDB->table("clay");
  //EXPECT_NO_THROW(clay_table->list_available_elems());
  //for(int i=1; i<120; i++){
  //  EXPECT_EQ(1,MDB->list_available_elems().count(i))
 // }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCDBTest, DISABLED_get_elem_row){
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
TEST_F(STCDBTest, DISABLED_availableData){
  // the DB should include K_d, S, D, etc. for each elem

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCDBTest, DISABLED_get_data_elem){
  // from a row object,
  // the DB should return a data element for any data type in the list of 
  // available data types
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCDBTest, DISABLED_get_temp_change){
  // the DB should give appropriate temperature changes for the materials
}


// MatDataTableTests.cpp
#include <gtest/gtest.h>
#include "MatDataTableTests.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, openDB){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, closeDB){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, listAvailableMats){
  // the DB should include one table for each important mat
  // Clay, Granite, Bedrock, Salt, Steel, Copper, Glass, Bentonite, UO2
  EXPECT_NO_THROW(db()->list_available_mats());
  EXPECT_EQ(1,db()->list_available_mats().count("clay"))
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, get_mat_table){
  // the DB should return a table for any mat in the list of avail mats
  set<std::string>::iterator it;
  for(it=db()->list_available_mats().begin();
      it!=db()->list_available_mats().end(); ++it){
    EXPECT_NO_THROW(db()->table((*it)))
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, listAvailableElems){
  // the DB should include all elements for each mat
  EXPECT_NO_THROW(db()->list_available_elems());
  for(int i=1; i<120; i++){
    EXPECT_EQ(1,db()->list_available_mats().count(i))
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, get_elem_row){
  // from a table object,
  // the DB should return a row for any iso in the list of avail isos
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, availableData){
  // the DB should include Kd, S, D, etc. for each iso
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, get_data_elem){
  // from a row object,
  // the DB should return a data element for any data type in the list of 
  // available data types
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, getS){
  // the DB should give appropriate solubility limits for canonical mat/isos, within a range.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, getKd){
  // the DB should give appropriate Kd values for canonical mat/isos, within a range.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, getD){
  // the DB should give appropriate D values for canonical mat/isos, within a range.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, setRefKd){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, setRefS){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, setRefD){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, infSolLimits){
  // Some elements can have effectively infinite solubility limits. 
  // Check that this is communicated.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, <++>){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, <++>){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, <++>){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, <++>){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, <++>){
}




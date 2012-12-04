// MaterialDBTests.cpp
#include <gtest/gtest.h>
#include "MaterialDBTests.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, openDB){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, closeDB){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, listAvailableMats){
  // the DB should include one table for each important mat
  // Clay, Granite, Bedrock, Salt, Steel, Copper, Glass, Bentonite, UO2
  EXPECT_NO_THROW(db()->list_available_mats());
  EXPECT_EQ(1,db()->list_available_mats().count("clay"))
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, get_mat_table){
  // the DB should return a table for any mat in the list of avail mats
  set<std::string>::iterator it;
  for(it=db()->list_available_mats().begin();
      it!=db()->list_available_mats().end(); ++it){
    EXPECT_NO_THROW(db()->table((*it)))
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, listAvailableElems){
  // the DB should include all elements for each mat
  MatDataTablePtr clay_table = db()->table("clay");
  EXPECT_NO_THROW(clay_table->list_available_elems());
  for(int i=1; i<120; i++){
    EXPECT_EQ(1,db()->list_available_elems().count(i))
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, get_elem_row){
  // from a table object,
  // the DB should return a row for any elem in the list of avail elems
  MatDataTablePtr clay_table = db()->table("clay");
  set<std::int>::iterator it;
  for(it=clay_table->list_available_elems().begin();
      it!=clay_table->list_available_elems().end(); ++it){
    EXPECT_NO_THROW(clay_table->data((*it)));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, availableData){
  // the DB should include Kd, S, D, etc. for each elem

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, get_data_elem){
  // from a row object,
  // the DB should return a data element for any data type in the list of 
  // available data types
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, getS){
  // the DB should give appropriate solubility limits for canonical mat/elems, within a range.
  EXPECT_NO_THROW(db()->getS("clay", 92));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, getKd){
  // the DB should give appropriate Kd values for canonical mat/elems, within a range.
  EXPECT_NO_THROW(db()->getKd("clay", 92));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, getD){
  // the DB should give appropriate D values for canonical mat/elems, within a range.
  EXPECT_NO_THROW(db()->getD("clay", 92));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, infSolLimits){
  // Some elements can have effectively infinite solubility limits. 
  // Check that this is communicated.
  EXPECT_NO_THROW(db()->getS("clay", 53));
}



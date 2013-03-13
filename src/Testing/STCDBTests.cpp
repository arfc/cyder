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
TEST_F(STCDBTest, get_mat_table){
 // the DB should return a table for a reasonable mat
 EXPECT_NO_THROW(SDB->table(salt_struct_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCDBTest, iso){
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


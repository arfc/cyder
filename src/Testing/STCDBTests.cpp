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
TEST_F(STCDBTest, getMatTable){
 // the DB should return a table for a reasonable mat
 EXPECT_NO_THROW(SDB->table(salt_struct_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCDBTest, stcFromMatTopeTime){
 EXPECT_NO_THROW(SDB->stc(salt_struct_, Am241_, 2));
 EXPECT_GT(SDB->stc(salt_struct_, Am241_, 2),0);
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


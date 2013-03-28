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
TEST_F(MaterialDBTest, get_mat_table){
  EXPECT_NO_THROW(MDB->table("clay", 0, 0, 0));
  EXPECT_NO_THROW(MDB->table("clay", 1, 1, 1));
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
  for(it=elem_ids_.begin(); it<elem_ids_.end(); it++){
    EXPECT_NO_THROW(MDB->S("clay", (*it)));
    EXPECT_GT(MDB->S("clay", (*it)),0);
  }

  EXPECT_FLOAT_EQ(0, MDB->table("clay", 0, 0, 0)->S(1));
  EXPECT_FLOAT_EQ(1, MDB->table("clay", 1, 1, 1)->S(1));

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, K_d){
  // the DB should give appropriate K_d values for canonical mat/elems, within a range.
  std::vector<Elem>::iterator it;
  for(it=elem_ids_.begin(); it<elem_ids_.end(); it++){
    EXPECT_NO_THROW(MDB->K_d("clay", (*it)));
    EXPECT_GT(MDB->K_d("clay", (*it)),0);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, D){
  // the DB should give appropriate D values for canonical mat/elems, within a range.
  std::vector<Elem>::iterator it;
  for(it=elem_ids_.begin(); it<elem_ids_.end(); it++){
    EXPECT_NO_THROW(MDB->D("clay", (*it)));
    EXPECT_GT(MDB->D("clay", (*it)),0);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, infSolLimits){
  // Some elements can have effectively infinite solubility limits. 
  // Check that this is communicated.
  EXPECT_NO_THROW(MDB->S("clay", 53));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, tableID){
  EXPECT_EQ("clay0", MDB->tableID("clay",1,1,1) );
  EXPECT_EQ("clay1", MDB->tableID("clay",2,1,1) );
  EXPECT_EQ("clay2", MDB->tableID("clay",3,1,1) );
  EXPECT_EQ("clay3", MDB->tableID("clay",4,1,1) );
  EXPECT_EQ("clay0", MDB->tableID("clay",1,1,1) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, initialized){
  EXPECT_NO_THROW(MDB->table("clay", 18, 1, 1));
  EXPECT_FALSE(MDB->initialized(MDB->tableID("clay",19,1,1)));
  EXPECT_NO_THROW(MDB->table("clay", 19, 1, 1));
  EXPECT_TRUE( MDB->initialized(MDB->tableID("clay",19,1,1)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MaterialDBTest, initializeFromSQL) {
  EXPECT_NO_THROW(MDB->initializeFromSQL("clay", 18, 1, 1));
}

// MatDataTableTests.h
#include <gtest/gtest.h>
#include "MatDataTable.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MatDataTableTest : public ::testing::Test {
  protected:
    std::vector<std::string> mat_names_;
    std::vector<int> elem_ids_;
    Elem h_, u_, am_, th_, pb_, one_g_; 
    MatDataTablePtr mat_table_;
    MatDataTablePtr default_table_;
    double ref_disp_;
    double ref_sol_;
    double ref_kd_;
    std::string mat_;
    std::vector<element_t> elem_vec_;
    std::map<Elem, int> elem_index_;
    std::map<Elem, double> d_;
    std::map<Elem, double> kd_;
    std::map<Elem, double> sol_;


    std::vector<element_t> initElemVec(){
      std::vector<int>::iterator it;
      for(it= elem_ids_.begin(); it!= elem_ids_.end(); ++it){
        Elem el = (*it);
        d_[el] = 1;
        kd_[el] = 1;
        sol_[el] = 1;
        element_t e = {el, d_[el] , kd_[el], sol_[el]};
        elem_vec_.push_back(e);
      }
      return elem_vec_;
    }

    std::map<Elem, int> initElemIndex(){
      std::vector<int>::iterator it;
      int i=0;
      for(it= elem_ids_.begin(); it!= elem_ids_.end(); ++it){
        Elem el = (*it);
        elem_index_.insert(std::make_pair(el, ++i));
      }
      return elem_index_;
    }

    void initMatTable(){
      mat_table_ = MatDataTablePtr(new MatDataTable(mat_, elem_vec_, 
            elem_index_, ref_disp_, ref_kd_, ref_sol_));
    };

    virtual void SetUp(){
      // composition set up
      h_ = 1;
      u_ = 92;
      am_ = 95;
      th_ = 90;
      pb_ = 82;
      one_g_ = 1.0;
      elem_ids_.push_back(h_);
      elem_ids_.push_back(u_);
      elem_ids_.push_back(th_);
      elem_ids_.push_back(am_);
      elem_ids_.push_back(pb_);
      ref_disp_=2;
      ref_kd_=2;
      mat_="clay";
      elem_vec_=initElemVec();
      elem_index_=initElemIndex();
      initMatTable();

      default_table_=MatDataTablePtr(new MatDataTable());
    }

    virtual void TearDown(){
    }
};

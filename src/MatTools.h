/*! \file MatTools.h
  \brief Declares the MatTools class used by the Generic Repositor 
  \author Kathryn D. Huff
 */
#if !defined(_MATTOOLS_H)
#define _MATTOOLS_H

#include <iostream>
#include "Logger.h"
#include <deque>
#include <vector>
#include <map>
#include <string>

#include "IsoVector.h"
#include "Material.h"

/** 
   @brief MatTools is a toolkit for manipulating materials. 
   **/
class MatTools { 
public:
  /**
     sums the materials in the vector, filling the vec and kg with 
     the cumulative IsoVector and total mass, in kg.
     @TODO should put this in a toolkit or something. Doesn't belong here.

     @param mats the vector of materials to be summed (not mixed)
    */
  static std::pair<IsoVector, double> sum_mats(std::deque<mat_rsrc_ptr> mats);

  /**
     removes the specified amount of the specified composition from a 
     deque of materials provided to the function by reference. 

     @param
     @param
     @param
    **/
  static void extract(const CompMapPtr comp_to_rem, double kg_to_rem, 
      std::deque<mat_rsrc_ptr>& mat_list);
};
#endif

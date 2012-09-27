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

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

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
};
#endif

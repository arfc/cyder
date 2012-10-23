/*! \file MatTools.cpp
    \brief Implements the MatTools class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <deque>
#include <time.h>
#include <assert.h>

#include "CycException.h"
#include "Logger.h"
#include "Timer.h"
#include "MatTools.h"
#include "Material.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> MatTools::sum_mats(deque<mat_rsrc_ptr> mats){
  IsoVector vec;
  CompMapPtr sum_comp = CompMapPtr(new CompMap(MASS));
  double kg = 0;
  double mass_to_add;

  if( mats.size() != 0 ){ 
    CompMapPtr comp_to_add;
    deque<mat_rsrc_ptr>::iterator mat;
    int iso;
    CompMap::const_iterator comp;

    for(mat = mats.begin(); mat != mats.end(); ++mat){ 
      kg += (*mat)->mass(MassUnit(KG));
      comp_to_add = (*mat)->isoVector().comp();
      comp_to_add->massify();
      for(comp = (*comp_to_add).begin(); comp != (*comp_to_add).end(); ++comp) {
        iso = comp->first;
        if(sum_comp->count(iso)!=0) {
          (*sum_comp)[iso] = (*sum_comp)[iso] + (comp->second)*kg;
        } else { 
          (*sum_comp)[iso] = (comp->second)*kg;
        }
      }
    }
  } else {
    (*sum_comp)[92235] = 0;
  }
  vec = IsoVector(sum_comp);
  return make_pair(vec, kg);
}

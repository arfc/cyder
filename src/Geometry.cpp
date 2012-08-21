/** \file Geometry.cpp
 * \brief Implements the Geometry class that defines the cylindrical extent of a 
 * component
 * \author Kathryn D. Huff
 */

#include <iostream>

#include "Geometry.h"
#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Geometry::Geometry() {
  inner_radius_ = 0; // 0 indicates a solid
  outer_radius_ = NULL; // NILL indicates an infinite object.
  point_t origin = {0,0,0}; 
  centroid_ = origin; // by default, the origin is the centroid

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Geometry::Geometry(Radius inner_radius, Radius outer_radius, point_t centroid) {
  inner_radius_ = inner_radius; 
  outer_radius_ = outer_radius; 
  centroid_ = centroid; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Geometry* Geometry::copy(Geometry* src, point_t centroid){
  // need a fresh central position for each geometry,
  // no two objects may have exactly the same properties.
  // http://plato.stanford.edu/entries/identity-indiscernible/
  Geometry* to_ret = new Geometry(src->inner_radius(),
      src->outer_radius(),
      centroid);
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Geometry::set_radius(BoundaryType boundary, Radius radius) { 
  switch(boundary){
    case INNER:
      inner_radius_ = radius;
      break;
    case OUTER:
      outer_radius_ = radius;
      break;
    default:
      throw CycException("Only INNER or OUTER radii may be set.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Geometry::set_centroid(point_t centroid) { 
  centroid_=centroid;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
const Radius Geometry::inner_radius(){return inner_radius_;}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
const Radius Geometry::outer_radius(){return outer_radius_;}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
const point_t Geometry::centroid(){return centroid_;}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
const double Geometry::x(){return (centroid()).x_;}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
const double Geometry::y(){return (centroid()).y_;}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
const double Geometry::z(){return (centroid()).z_;}

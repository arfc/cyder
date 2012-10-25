/** \file Geometry.h
 * \brief Declares the Geometry class for defining the cylindrical extent and 
 * position of the component
 * \author Kathryn D. Huff
 */
#if !defined(_GEOMETRY_H)
#define _GEOMETRY_H

#include "boost/shared_ptr.hpp"

/// type definition for Radius in meters
typedef double Radius;

/// type definition for Length in meters
typedef double Length;

/// type definition for Volume in meters^3
typedef double Volume;

/// Enum for type of boundary.
enum BoundaryType {INNER, OUTER};

/// type definition for an X, Y, Z point definition
typedef struct point_t{
  double x_; /**<The x coordinate of the centroid */
  double y_; /**<The y coordinate of the centroid */
  double z_; /**<The z coordinate of the centroid */
}point_t;

/// type definition for a shared pointer to a Geometry object
class Geometry;
typedef boost::shared_ptr<Geometry> GeometryPtr;


/**
   @brief The Geometry class stores the cylindrical Component geometry 

   The Geometry class holds information about the position, extent, and 
   structure of the component geometry.
 */
class Geometry {
  
public:
  /**
     Default Constructor
    */
  Geometry();

  /**
     Detailed Constructor

     @param inner_radius the inner_radius_ data member [m]
     @param outer_radius the outer_radius_ data member [m]
     @param centroid the centroid_ data member [m]
     @param length the length_ data memeber [m]
     @return a geometry object
    */
  Geometry(Radius inner_radius, Radius outer_radius, 
      point_t centroid, Length length);

  /**
     A copy constructor

     @param src the original Geometry object, to copy
     @param centroid a new central position for the new cylinder

     @return a copy of the src object
    */
  GeometryPtr copy(GeometryPtr src, point_t centroid);

  /**
     Set the radius of the surface at the boundary indicated. 
     inner radius is 0 for solid objects. [m]
     outer radius is null for infinite objects. [m]

     @param boundary the boundary at which to set the radius
     @param radius the new radius at the boundary [m]
    */
  void set_radius(BoundaryType boundary, Radius radius); 

  /**
     Set the coordinate location of the centroid, a point. [m]

     @param centroid the new central point of the cylinder
    */
  void set_centroid(point_t centroid); 

  /**
     Set the length_ datamember of the cylinder [m]

     @param length the new length of the cylinder 
     */
  void set_length(Length length); 

  /**
     Radius of the inner surface. 0 for solid objects. [m]

     @return the inner radius of the cylindrical geometry object
    */
  const Radius inner_radius(); 

  /**
     Returns the radius of the outer surface. 
     numeric_limits<double>::infinity for infinite objects.[m] 

     @return the outer radius of the cylindrical geometry object
    */
  const Radius outer_radius(); 

  /**
     Returns the coordinate location of the centroid, a point. [m]
    */
  const point_t centroid(); 

  /// get the x component of the centroid position vector of the object
  const double x();

  /// get the y component of the centroid position vector of the object
  const double y();

  /// get the z component of the centroid position vector of the object
  const double z();

  /// get the length of the component 
  const Length length();

  /** 
     get the volume of the component 

     @return numeric_limits<double>::infinity() if the volume is infinite
     */
  const Volume volume();

  /** 
     get the radial midpoint of the component 

     @return numeric_limits<double>::infinity() if the radius is infinite
    **/
  const Radius radial_midpoint();

protected:
  /**
     Returns the (solid) volume of a cylinder, not explicitly the one 
     represented by this geometry.
     
     @param radius the (outer) radius of the (solid) cylindrical volume [m]
     @param length the length of the cylindrical volume [m]
     @return the volume of a solid cylinder with radius=radius and length=length
    */
  Volume solid_volume(Radius radius, Length length); 

  /**
     Radius of the inner surface. 0 for solid objects. [m]
    */
  Radius inner_radius_; 

  /**
     Radius of the outer surface. numeric_limits<double>::infinity() 
     for infinite objects. [m]
    */
  Radius outer_radius_; 

  /**
     The coordinate location of the centroid, a point. [m]
    */
  point_t centroid_; 

  /** 
     The length of the cylindrical section [m]
     numeric_limits<double>::infinity() if infinite
    */
  Length length_;
};


#endif

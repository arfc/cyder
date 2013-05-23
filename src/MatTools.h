/*! \file MatTools.h
  \brief Declares the MatTools class used by the Generic Repository 
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
#define SECSPERMONTH 2629740.0
/**
   type definition for radiotoxicity in Sv
  */
typedef double Tox;

/**
   type definition for Concentration Gradients in kg/m^o4
 */
typedef double ConcGrad;

/**
   type definition for a map from isotopes to concentrations
   The keys are the isotope identifiers Z*1000 + A
   The values are the Concentration Gradients for each isotope
  */
typedef std::map<int, ConcGrad> ConcGradMap;

/**
   type definition for Concentrations in kg/m^3
 */
typedef double Concentration;

/**
   type definition for a map from isotopes to concentrations
   The keys are the isotope identifiers Z*1000 + A
   The values are the Concentrations of each isotope [kg/m^3]
  */
typedef std::map<int, Concentration> IsoConcMap;

/**
   type definition for Fluxes in kg/m^2s
 */
typedef double Flux;

/**
   type definition for a map from isotopes to concentrations
   The keys are the isotope identifiers Z*1000 + A
   The values are the Fluxes of each isotope [kg/m^2s]
  */
typedef std::map<int, Flux> IsoFluxMap;


/** 
   @brief MatTools is a toolkit for manipulating materials. 
   **/
class MatTools { 
public:
  /**
     sums the materials in the deque of mats, filling the vec and kg with 
     the cumulative IsoVector and total mass, in kg.

     @param mats the vector of materials to be summed (not mixed)
    */
  static std::pair<IsoVector, double> sum_mats(std::deque<mat_rsrc_ptr> mats);

  /**
     sums the materials in the vector in an intelligent way, to avoid floating point issues.

     @param input is the list of values to add to each other
     @return is the sum of all the values in the input vector
     */
  static double KahanSum(std::vector<double> input);

  /**
     removes the specified amount of the specified composition from a 
     deque of materials provided to the function by reference. 

     @param comp_to_rem a CompMapPtr representing what to remove
     @param kg_to_rem a mass to remove of the CompMapPtr [kg]
     @param mat_list the list of materials to remove that comp from
     @param threshold the amount (in kg) that should be considered negligible

     @return the material extracted
    **/
  static mat_rsrc_ptr extract(const CompMapPtr comp_to_rem, 
      double kg_to_rem, std::deque<mat_rsrc_ptr>& mat_list, double threshold=0);

  /**
    Converts a CompMap and associated total mass to an IsoConcMap for a Volume

    @param comp the composition to convert, a CompMapPtr
    @param mass the total mass of the composition [kg]
    @param vol the total volume in which the concentration exists [m^3]

    @return an IsoConcMap whose elements are comp[iso]*mass/volume
  */  
  static IsoConcMap comp_to_conc_map(const CompMapPtr comp, 
      double mass, double vol); 

  /**
    Converts an IsoConcMap and Volume to CompMap and associated total mass

    @param conc the IsoConcMap to convert
    @param vol the total volume in which the concentration exists [m^3]

    @return a CompMapPtr and associated mass 
  */  
  static std::pair<CompMapPtr, double> conc_to_comp_map(const 
      IsoConcMap conc, double vol); 

  /// Returns an empty IsoConcMap
  static IsoConcMap zeroConcMap();

  /**
    Returns the fluid volume [m^3] based on the total volume and the porosity

    @param V_T the total volume [m^3]
    @param theta the porosity (a fraction)

    @return V_f the fluid volume [m^3]
    */
  static double V_f(double V_T, double theta);

  /**
    Returns the free fluid volume [m^3] based on the 
    total volume, porosity, and total degradation

    @param V_T the total volume [m^3]
    @param theta the porosity (a fraction)
    @param d the total degradation (a fraction)

    @return V_ff the free fluid volume [m^3]
    */
  static double V_ff(double V_T, double theta, double d);

  /**
    Returns the fluid volume in the intact matrix [m^3] based on the 
    total volume, porosity, and total degradation

    @param V_T the total volume [m^3]
    @param theta the porosity (a fraction)
    @param d the total degradation (a fraction)

    @return V_mf the fluid volume in the intact matrix [m^3]
    */
  static double V_mf(double V_T, double theta, double d);

  /**
    Returns the solid volume [m^3] based on the total volume and the porosity

    @param V_T the total volume [m^3]
    @param theta the porosity (a fraction)

    @return V_s the solid volume [m^3]
    */
  static double V_s(double V_T, double theta);

  /**
    Returns the degraded solid volume [m^3] based on the total volume and the 
    porosity

    @param V_T the total volume [m^3]
    @param theta the porosity (a fraction)

    @return V_ds the degraded solid volume [m^3]
    */
  static double V_ds(double V_T, double theta, double d);

  /**
    Returns the matrix solid volume [m^3] based on the total volume and the 
    porosity

    @param V_T the total volume [m^3]
    @param theta the porosity (a fraction)

    @return V_ms the matrix solid volume [m^3]
    */
  static double V_ms(double V_T, double theta, double d);

  /**
     Confirms whether or not the provided value is a percent (0<=per<=1)

     @param per the value to test
     @throws CycRangeException
    */
  static void validate_percent(double per);

  /**
     Confirms whether or not the provided value is finite and 
     positive(0<=pos<=infty)

     @param pos the value to test
     @throws CycRangeException
    */
  static void validate_finite_pos(double pos);

  /**
     Confirms whether or not the provided value is positive(0<=pos)

     @param pos the value to test
     @throws CycRangeException if pos<0
    */
  static void validate_pos(double pos);

  /**
     Confirms whether or not the provided value is zero 

     @param nonzero the value to test
     @throws CycRangeException
    */
  static void validate_nonzero(double nonzero);

  /**
    This is a helper function that scales an IsoConcMap with a scalar

    @param C_0 the original IsoConcMap, to be scaled.
    @param scalar the scalar by which to multiply each element of C_0 [-]
    */
  static IsoConcMap scaleConcMap(IsoConcMap C_0, double scalar);

  /**
    This is a helper function that returns the sum of two IsoConcMaps

    @param orig the original IsoConcMap (not modified in this function)
    @param to_add  IsoConcMap to add (not modified in this function)

    @return to_ret the sum of orig and to_add
    */
  static IsoConcMap addConcMaps(IsoConcMap orig, IsoConcMap to_add);

  /// @TODO add comments
  static CompMapPtr addCompMaps(CompMapPtr orig, CompMapPtr to_add);

  /// @TODO add comments
  static std::pair<CompMapPtr, double> subtractCompMaps(std::pair<CompMapPtr, 
      double> orig, std::pair<CompMapPtr, double> to_subtract);

  /** 
    Converts a standard isoid into an element. 

    @param iso the isotope id (i.e. 92235) to convert

    @return elem the element id (i.e. 92) that corresponds to iso
    */
  static int isoToElem(int iso);

  /// @TODO add comments
  static std::vector<double> linspace(double a, double b, int n);
  
};
#endif

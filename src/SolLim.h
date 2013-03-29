/*! \file SolLim.h
  \brief Declares the SolLim class used by the Generic Repository 
  \author Kathryn D. Huff
 */
#if !defined(_SOLLIM_H)
#define _SOLLIM_H

#include <iostream>
#include "Logger.h"
#include <deque>
#include <vector>
#include <map>
#include <string>

#include "IsoVector.h"
#include "Material.h"

/** 
   @brief SolLim is a toolkit for manipulating materials under 
   solubility limited conditions 
   **/
class SolLim { 
public:
  /**
    Returns m_f the contaminant mass in the fluid volume [kg]

    @param m_T the total mass of the element e
    @param K_d the distribution coefficient [m^3/kg]
    @param V_s the solid volume [m^3]
    @param V_f the fluid volume [m^3]

    @return m_f the contaminant mass in the fluid volume [kg]
    */
  static double m_f(double m_T, double K_d, double V_s, double V_f);

  /**
    Returns m_s the mass in the solid volume[kg]

    @param m_T the total mass of the element e
    @param K_d the distribution coefficient [m^3/kg]
    @param V_s the solid volume [m^3]
    @param V_f the fluid volume [m^3]

    @return m_s the contaminant mass in the solid volume[kg]
    */
  static double m_s(double m_T, double K_d, double V_s, double V_f);

  /**
    Returns m_ds the contaminant mass in the degraded solid [kg]

    @param m_T the total mass of the element e
    @param K_d the distribution coefficient [m^3/kg]
    @param V_s the solid volume [m^3]
    @param V_f the fluid volume [m^3]
    @param d the amount this component has degraded (a fraction)

    @return m_ds the contaminant mass in the degraded solid [kg]
    */
  static double m_ds(double m_T, double K_d, double V_s, double V_f, double d);

  /**
    Returns m_ms the mass in the intact matrix solid volume [kg]

    @param m_T the total mass of the element e
    @param K_d the distribution coefficient [m^3/kg]
    @param V_s the solid volume [m^3]
    @param V_f the fluid volume [m^3]
    @param d the amount this component has degraded (a fraction)

    @return m_ms the mass in the intact matrix solid volume [kg]
    */
  static double m_ms(double m_T, double K_d, double V_s, double V_f, double d);

  /**
    Returns m_ff the contaminant mass in the free fluid volume [kg]

    @param m_T the total mass of the element e
    @param K_d the distribution coefficient [m^3/kg]
    @param V_s the solid volume [m^3]
    @param V_f the fluid volume [m^3]
    @param d the amount this component has degraded (a fraction)

    @return m_ff the contaminant mass in the free fluid volume [kg]
    */
  static double m_ff(double m_T, double K_d, double V_s, double V_f, double d);

  /**
    Returns m_mf the contaminant mass in the matrix pore fluid volume [kg]

    @param m_T the total mass of the element e
    @param K_d the distribution coefficient [m^3/kg]
    @param V_s the solid volume [m^3]
    @param V_f the fluid volume [m^3]
    @param d the amount this component has degraded (a fraction)

    @return m_mf the contaminant mass in the matrix pore fluid volume [kg]
    */
  static double m_mf(double m_T, double K_d, double V_s, double V_f, double d);

  /**
    Returns m_aff the available contaminant mass in the free fluid volume [kg]

    @param m_ff the contaminant mass int he free fluid volume [kg]
    @param V_ff the free fluid volume [m^3]
    @param C_sol the solubility limit of the element e [kg/m^3]

    @return m_aff the available contaminant mass in the free fluid volume [kg]
    */
  static double m_aff(double m_ff, double V_ff, double C_sol);

  /**
    Returns m_ps the contaminant mass that has precipitated into a solid form [kg]

    @param m_T the total mass of the element e
    @param K_d the distribution coefficient [m^3/kg]
    @param V_s the solid volume [m^3]
    @param V_f the fluid volume [m^3]
    @param d the amount this component has degraded (a fraction)
    @param C_sol the solubility limit of the element e [kg/m^3]

    @return m_ps the contaminant mass that has precipitated into a solid form [kg]
    */
  static double m_ps(double m_T, double K_d, double V_s, double V_f, double d, double C_sol);
};
#endif

#!/bin/bash
cd input
rm -r deg
python perturb.py -i deg.xml.in -o ./deg/ -p degradation -l 0.0001 -u 0.1001 -n 101
rm -r kd
python perturb.py -i kd.xml.in -o ./kd/ -p ref_kd -l 0.0001 -u 1.0001 -n 101
rm -r sol
python perturb.py -i sol.xml.in -o ./sol/ -p ref_sol_lim -l 0.001 -u 10.001 -n 101
rm -r sol_nbc
python perturb.py -i sol_nbc.xml.in -o ./sol_nbc/ -p ref_sol_lim -l 0.001 -u 10.001 -n 101
rm -r deg_nbc
python perturb.py -i deg_nbc.xml.in -o ./deg_nbc/ -p degradation -l 0.0001 -u 0.1001 -n 101
rm -r lpDM
python perturb.py -i lpDM.xml.in -o ./lpDM/ -p peclet -l 0.01 -u 1.0 -n 101
rm -r lpDM_t_t
python perturb.py -i lpDM.xml.in -o ./lpDM_t_t/ -p transit_time -l 1. -u 101.  -n 101
rm -r lpEXPM_t_t
python perturb.py -i lpEXPM.xml.in -o ./lpEXPM_t_t/ -p transit_time -l 1. -u 101.  -n 101
rm -r lpPFM_t_t
python perturb.py -i lpPFM.xml.in -o ./lpPFM_t_t/ -p transit_time -l 1. -u 101.  -n 101
rm -r deg_cbc
python perturb.py -i deg_cbc.xml.in -o ./deg_cbc/ -p ref_disp advective_velocity -l 1e-9  1e-14 -u 1e-8 1e-13 -n 5 5 
rm -r od 
python perturb.py -i od.xml.in -o ./od/ -p ref_disp advective_velocity -l 1e-9 1e-14 -u 1e-8 1e-14 -n 5 5
rm -r kd_nbc
python perturb.py -i kd_nbc.xml.in -o ./kd_nbc/ -p ref_kd -l 0.0001 -u 1.0001 -n 101


cd kd_nbc
python ../../output/line_plot.py -r 'kd_nbc' -xp 'ref_kd' -xl 'Reference Distribution Coefficient' -yl 'Mass in Far Field $[kg]$' -t 'Reference Distribution Coefficient Sensitivity' -o 'kd_nbc.eps'
cd ../sol_nbc
python ../../output/line_plot.py -r 'sol_nbc' -xp 'ref_sol' -xl 'Reference Solubility Limit $[kg/m^3]$' -yl 'Mass in Far Field $[kg]$' -t 'Reference Solubility Limit Sensitivity' -o 'sol_nbc.eps'
cd ../deg_nbc
python ../../output/line_plot.py -r 'deg_nbc' -xp 'degradation' -xl 'Degradation Rate $[\%/month]$' -yl 'Mass in Far Field $[kg]$' -t 'Degradation Rate Sensitivity' -o 'deg_nbc.eps'
cd kd
python ../../output/line_plot.py -r 'kd' -xp 'ref_kd' -xl 'Reference Distribution Coefficient' -yl 'Mass in Far Field $[kg]$' -t 'Reference Distribution Coefficient Sensitivity' -o 'kd.eps'
cd ../sol
python ../../output/line_plot.py -r 'sol' -xp 'ref_sol' -xl 'Reference Solubility Limit $[kg/m^3]$' -yl 'Mass in Far Field $[kg]$' -t 'Reference Solubility Limit Sensitivity' -o 'sol.eps'
cd ../deg
python ../../output/line_plot.py -r 'deg' -xp 'degradation' -xl 'Degradation Rate $[\%/month]$' -yl 'Mass in Far Field $[kg]$' -t 'Degradation Rate Sensitivity' -o 'deg.eps'
cd ../lpDM
python ../../output/line_plot.py -r 'lpDM' -xp 'peclet' -xl 'Peclet Number $[-]$' -yl 'Mass in Far Field $[kg]$' -t 'Peclet Number Sensitivity' -o 'lpDM.eps'
cd ../lpDM_t_t
python ../../output/line_plot.py -r 'lpDM' -xp 'transit_time' -xl 'Transit Time $[month]$' -yl 'Mass in Far Field $[kg]$' -t 'Transit Time Sensitivity' -o 'lpDM_t_t.eps'
cd ../lpEXPM_t_t
python ../../output/line_plot.py -r 'lpEXPM' -xp 'transit_time' -xl 'Transit Time $[month]$' -yl 'Mass in Far Field $[kg]$' -t 'Transit Time Sensitivity' -o 'lpEXPM_t_t.eps'
cd ../lpPFM_t_t
python ../../output/line_plot.py -r 'lpPFM' -xp 'transit_time' -xl 'Transit Time $[month]$' -yl 'Mass in Far Field $[kg]$' -t 'Transit Time Sensitivity' -o 'lpPFM_t_t.eps'


cd ../deg_cbc
python ../../output/contour_plot.py -r 'deg_cbc' -xp 'ref_disp' -xl 'Reference Diffusivity $[m^2/s]$' -yp 'advective_velocity' -yl 'Vertical Advective Velocity $[m/s]$' -zl 'Mass in Far Field $[kg]$' -n 200 -t 'Sensitivity to Advection and Diffusion Coeffients' -o 'cbc_deg.eps'

cd ../od
python ../../output/contour_plot.py -r 'od' -xp 'ref_disp' -xl 'Reference Diffusivity $[m^2/s]$' -yp 'advective_velocity' -yl 'Vertical Advective Velocity $[m/s]$' -zl 'Mass in Far Field $[kg]$' -n 200 -t 'Advection/Diffusion Transition OneDimPPM' -o 'od.eps'

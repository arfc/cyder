#!/bin/bash
cd input
rm -r kd
python perturb.py -i kd.xml.in -o ./kd/ -p ref_kd -l 0.0001 -u 1.0001 -n 101
rm -r sol
python perturb.py -i sol.xml.in -o ./sol/ -p ref_sol_lim -l 0.0000001 -u .0010001 -n 101
rm -r deg
python perturb.py -i deg.xml.in -o ./deg/ -p degradation -l 0.0001 -u 0.1001 -n 101
rm -r lpDM
python perturb.py -i lpDM.xml.in -o ./lpDM/ -p peclet -l 0.01 -u 1.0 -n 101
rm -r lpDM_t_t
python perturb.py -i lpDM.xml.in -o ./lpDM_t_t/ -p transit_time -l 1. -u 101.  -n 101
rm -r lpEXPM_t_t
python perturb.py -i lpEXPM.xml.in -o ./lpEXPM_t_t/ -p transit_time -l 1. -u 101.  -n 101
rm -r lpPFM_t_t
python perturb.py -i lpPFM.xml.in -o ./lpPFM_t_t/ -p transit_time -l 1. -u 101.  -n 101


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

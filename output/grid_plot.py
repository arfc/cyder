from __future__ import with_statement
from matplotlib.mlab import griddata
import matplotlib.cm as cm
import matplotlib.colors as colors
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.font_manager as font_manager
import pdb, sys
import subprocess as sp



''' This function converts a string to a number '''
def convertStr(s):
  try:
    ret = int(s)
  except ValueError:
    #Try float.
    ret = float(s)
  return ret


''' This function plots just the geometry of the nodes'''
def plotGeom():
  # plot data points.
  plt.scatter(x,y,marker='o',c='b',s=5,zorder=10)
  plt.xlim(-1,xlimit)
  plt.ylim(-ylimit,ylimit)
  plt.savefig('geom.png')


''' This function plots just the geometry of the nodes'''
def plotHeat():
  # open the DataFile.csv
  with open(sys.argv[2]) as dat:
    # read the first line and pull out the years
    years = map(convertStr, dat.readline().strip(',\n').split(',')[1:])
    # prepare the datatype
    # 1 int for the node, and then as many floats as years 
    dattype = [('nodes', np.int64)] + [('',np.float64)]*len(years)
    # load the file (minus the first line)
    hdata = np.loadtxt(dat, delimiter=',', dtype=dattype)
  # map the resulting structure 
  hdata = hdata.view(np.dtype([('nodes',np.int64), ('heat', np.float64, len(years))]))
  heat = hdata['heat']
  nodes = hdata['nodes']
  # define grid.
  xi = np.linspace(-max,max,2*max)
  yi = np.linspace(-max,max,2*max)
  # choose a year to plot
  for yr in years:
    plt.clf()
    plotGeom()
    # figure out which year
    yrToPlt = np.float64(yr)
    index =np.nonzero(years == yrToPlt)[0][0]
    # define the z data, heat, in the order of the xyznodes
    z=np.zeros(len(xyznodes))
    i=0
    for num in xyznodes:
      j = np.nonzero(nodes == num)[0][0]
      #z[i]=heat[j][np.nonzero(years==year)[0][0]]
      z[i]=heat[j][index]
      i=i+1
    # grid the data.
    zi = griddata(x,y,z,xi,yi)
    scale = colors.normalize(1,heat.max())
    # contour the gridded data, plotting dots at the nonuniform data points.
    CS = plt.contour(xi,yi,zi,10,linewidths=0.5,colors='k')
    CS = plt.contourf(xi,yi,zi,10,cmap=plt.cm.jet, norm=scale)
    # draw colorbar
    plt.colorbar() 
    plt.xlim(-1,xlimit)
    plt.ylim(-ylimit,ylimit)
    plt.title('Heat during year %d'%yrToPlt)
    if yrToPlt < 10 :
      plt.savefig('heat/heat0000%d.png'%yrToPlt)
    elif yrToPlt < 100 :
      plt.savefig('heat/heat000%d.png'%yrToPlt)
    elif yrToPlt < 1000 :
      plt.savefig('heat/heat00%d.png'%yrToPlt)
    elif yrToPlt < 10000 :
      plt.savefig('heat/heat0%d.png'%yrToPlt)
    elif yrToPlt < 10000 :
      plt.savefig('heat/heat%d.png'%yrToPlt)
    elif yrToPlt >= 100000 :
      sys.exit("Too many years, please shorten input files.")
  # build a movie
  retcode = sp.call(["convert", "-set delay 24 /heat/heat*.png bob.gif"])
  # use return code to throw meaningful error? 


''' This function decides which type of plot to make'''
def pickPlot():
  if plottype == 'geom' :
    plotGeom()
  elif plottype == 'heat':
    plotHeat()


''' SET UP '''
# get files from input args
if len(sys.argv) < 2:  
  # stop the program and print an error message
  sys.exit("Too few arguments. Usage: grid_plot.py XYZfile.csv [DataFile.csv]")
# figure out which type of plot to plot.
if len(sys.argv) < 3:
  # assume we want to just plot the geometry
  plottype='geom'
elif len(sys.argv) == 3:
  # assume we want to plot the heat data
  plottype='heat'
elif len(sys.argv) == 5:
  plottype='heat'
elif len(sys.argv) > 3:
  # stop the program and print an error message
  sys.exit("Too many arguments: Usage: grid_plot.py XYZfile.csv [DataFile.csv]")

# open XYZfile.csv 
with open(sys.argv[1]) as xyz:
  # read the first line and pull out the title
  title = xyz.readline().strip('\n').strip(',')
  # read the next line and pull out the column names
  cols = xyz.readline().strip(',\n').split(',')[1:]
  # prepare the datatype
  xyztype = [('nodes', np.int64)] + [('x', np.float64)] + [('y', np.float64)] + [('z', np.float64)] 
  # load the file (minus the first line)
  xyzdata = np.loadtxt(xyz, delimiter=',', dtype=xyztype)
# map the resulting structure 
xyznodes = xyzdata['nodes']
x = xyzdata['x']
y = xyzdata['y']
z = xyzdata['z']
# plot the whole range
xmax = x.max()
ymax = y.max()
xymax = np.array([xmax, ymax])
max = int(xymax.max())
if len(sys.argv) == 5:
  xlimit = convertStr(sys.argv[3])
  ylimit = convertStr(sys.argv[4])
else :
  xlimit = xmax
  ylimit = ymax

pickPlot()


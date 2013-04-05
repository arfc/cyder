


"""
Comparison of griddata and tricontour for an unstructured triangular grid.
"""
from __future__ import print_function
import matplotlib.pyplot as plt
from pylab import *
import matplotlib.tri as tri
import numpy as np
from numpy.random import uniform, seed
from matplotlib.mlab import griddata
import time

class ContourPlot(object) :
    """
    A class representing a contour plot. It needs data, axis labels, a title, 
    etc.
    """
    _x_min = -2 
    _y_min = -2
    _x_max = 2
    _y_max = 2
    _x = None
    _y = None
    _z = None
    _npts = 200
    _n_labels = 15 # the number of labels on each axis

    def __init__():
        _x=get_x()
        _y=get_y()
        _z=get_z()



# making the data
seed(0)
ngridx = 100 # xresolution
ngridy = 200 # yresolution

# griddata and contour.
start = time.clock()
plt.subplot(211)
spacing = 1./ngridx
xi = np.linspace(_x_min-spacing, _x_max+spacing, ngridx)
yi = np.linspace(_y_min-spacing, y_may+spacing, ngridy)
zi = griddata(x, y, z, xi, yi, interp='linear')
plt.contour(xi, yi, zi, n_labels, linewidths=0.5, colors='k')
plt.contourf(xi, yi, zi, n_labels, cmap=plt.cm.rainbow,
             norm=plt.normalize(vmax=abs(zi).max(), vmin=-abs(zi).max()))
plt.colorbar() # draw colorbar
plt.plot(x, y, 'ko', ms=3)
plt.xlim(_x_min, _x_max)
plt.ylim(_y_min, _y_max)
plt.title('griddata and contour (%d points, %d grid points)' % (npts, ngridx*ngridy))
print ('griddata and contour seconds: %f' % (time.clock() - start))

# tricontour.
start = time.clock()
plt.subplot(212)
triang = tri.Triangulation(x, y)
plt.tricontour(x, y, z, n_labels, linewidths=0.5, colors='k')
plt.tricontourf(x, y, z, n_lables, cmap=plt.cm.rainbow,
                norm=plt.normalize(vmax=abs(zi).max(), vmin=-abs(zi).max()))
plt.colorbar()
plt.plot(x, y, 'ko', ms=3)
plt.xlim(_x_min, _x_max)
plt.ylim(_y_min, _y_max)
plt.title(get_title(npts))
print ('tricontour seconds: %f' % (time.clock() - start))

savefig(get_filename())

def get_x() :
    xdata = uniform(_x_min, _x_max, npts) # this should be a matrix of identical columns?
    return xdata;

def get_y() :
    ydata = uniform(_y_min, _y_max, npts) # this should be a matrix of identical columns?
    return ydata;

def get_z() :
    zdata = x*np.exp(-x**2-y**2)  # this should be the mass in whatever component
    return zdata;

def get_title(npts) :
    return 'tricontour (%d points)' % npts

def get_filename() : 
    return 'contour_plot.eps'

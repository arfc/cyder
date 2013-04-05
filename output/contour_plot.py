


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

class ContourPlot(object) :
    """
    A class representing a contour plot. It needs data, a_xis labels, a title, 
    etc.
    """
    _x_min = -2 
    """
    <++>
    """
    _y_min = -2
    """
    <++>
    """
    _x_max = 2
    """
    <++>
    """
    _y_max = 2
    """
    <++>
    """
    _x = None
    """
    <++>
    """
    _y = None
    """
    <++>
    """
    _z = None
    """
    <++>
    """
    _xi = None
    """
    <++>
    """
    _yi = None
    """
    <++>
    """
    _zi = None
    """
    <++>
    """
    _npts = 200
    """
    <++>
    """
    _n_labels = 15 # the number of labels on each a_xis
    """
    <++>
    """
    _filename = "out.eps"
    """
    <++>
    """
    _title = "plot_title"
    """
    <++>
    """


    def __init__(self):
        self._x = self.set_x()
        self._y = self.set_y()
        self._z = self.set_z()
        self._xi = self.set_xi(100)
        self._yi = self.set_yi(200)
        
        self._title = get_title(self._npts)
        self._filename = get_filename()
        
        self.grid_data_and_contour()
        self.plot_tricontour()
        self.save_it()

    def grid_data_and_contour(self) :
        # griddata and contour.
        x=self._x 
        y=self._y 
        z=self._z
        n_labels=self._n_labels
        xi=self._xi
        yi=self._yi
        zi=self._zi
        x_max=self._x_max 
        y_max=self._y_max
        grid_data_and_contour(x,y,z,xi,yi,n_labels,zi,x_min,x_max,y_min_y_max)

    def grid_data_and_contour(x,y,z,xi,yi,n_labels,zi,x_min,x_max,y_min_y_max):
        plt.subplot(211)
        zi = griddata(x, y, z, xi, yi, interp='linear')
        plt.contour(xi, yi, zi, n_labels, linewidths=0.5, colors='k')
        plt.contourf(xi, yi, zi, n_labels, cmap=plt.cm.rainbow,
                     norm=plt.normalize(vmax=abs(zi).max(), vmin=-abs(zi).max()))
        plt.colorbar() # draw colorbar
        plt.plot(x, y, 'ko', ms=3)
        plt._xlim(x_min, x_max)
        plt._ylim(y_min, y_max)
        print ('griddata and contour plotted')

    def plot_tricontour(self):
        # tricontour.
        x=self._x 
        y=self._y 
        z=self._z
        n_labels=self._n_labels
        zi=self._zi
        x_max=self._x_max 
        y_max=self._y_max
        title=self._title
        plot_tricontour(x,y,z,n_labels,x_min, x_max, y_min, y_max, zi, title)

    def plot_tricontour(x,y,z,n_labels,x_min, x_max, y_min, y_max, zi, title):
        plt.subplot(212)
        triang = tri.Triangulation(x, y)
        plt.tricontour(x, y, z, n_labels, linewidths=0.5, colors='k')
        plt.tricontourf(x, y, z, n_lables, cmap=plt.cm.rainbow,
                        norm=plt.normalize(vmax=abs(zi).max(), vmin=-abs(zi).max()))
        plt.colorbar()
        plt.plot(x, y, 'ko', ms=3)
        plt._xlim(x_min, x_max)
        plt._ylim(y_min, y_max)
        plt.title(title)
        print ('tricontour plotted')

    def save_it(self) :
        savefig(self._filename)
        print ('saved as '+ self._filename)

    def set_x(self) :
        if self._x is not None :
            seed(0)
            # this should be a matrix of identical columns?
            self._x = uniform(self._x_min, self._x_max, self._npts) 
        return self._x;

    def set_xi(self, ngridx) :
        if self._xi is not None :
            spacing = 1./ngridx
            self._xi = np.linspace(self._x_min-spacing, self._x_max+spacing, ngridx)
        return self._xi

    def set_y(self) :
        if self._x is not None :
            seed(0)
            # this should be a matrix of identical columns
            self._y = uniform(self._y_min, self._y_max, self._npts) 
        return self._y

    def set_yi(self, ngridy) :
        if self._yi is not None :
            spacing = 1./ngridy
            self._yi = np.linspace(self._y_min-spacing, self._y_max+spacing, ngridy)
        return self._yi

    def set_z(self) :
        if self._z is not None :
            seed(0)
            # this should be the mass in whatever component
            self._z = self._x*np.exp(-self._x**2-self._y**2)  
        return self._z;

    def title(npts) :
        title = 'tricontour (%d points)' % npts
        return title

    def filename() : 
        filename = 'contour_plot.eps'
        return filename


if __name__=="__main__" :
    ContourPlot()

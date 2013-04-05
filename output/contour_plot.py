


"""
Comparison of griddata and tricontour for an unstructured triangular grid.
"""
from __future__ import print_function
import matplotlib.p_yplot as plt
from p_ylab import *
import matplotlib.tri as tri
import nump_y as np
from nump_y.random import uniform, seed
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
        self._x = get_x()
        self._y = get_y()
        self._z = get_z()
        self._xi = get_xi(100, self._x_min, self._x_max)
        self._yi = get_yi(200, self._y_min, self._y_max)
        self._zi = get_zi()
        
        self._title = get_title(_npts)
        self._filename = get_filename()
        
        self.grid_data_and_contour()
        self.plot_tricontour()

    def grid_data_and_contour(self) :
        # griddata and contour.
        plt.subplot(211)
        zi = griddata(_x, _y, _z, _xi, _yi, interp='linear')
        plt.contour(_xi, _yi, _zi, _n_labels, linewidths=0.5, colors='k')
        plt.contourf(_xi, _yi, _zi, _n_labels, cmap=plt.cm.rainbow,
                     norm=plt.normalize(vmax=abs(_zi).max(), vmin=-abs(_zi).max()))
        plt.colorbar() # draw colorbar
        plt.plot(_x, _y, 'ko', ms=3)
        plt._xlim(_x_min, _x_max)
        plt._ylim(_y_min, _y_max)
        print ('griddata and contour')

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

    def plot_tricontour(x,y,z,n_labels,x_min, x_max, y_min, y_max, zi, title)
        plt.subplot(212)
        triang = tri.Triangulation(_x, _y)
        plt.tricontour(_x, _y, _z, _n_labels, linewidths=0.5, colors='k')
        plt.tricontourf(_x, _y, _z, n_lables, cmap=plt.cm.rainbow,
                        norm=plt.normalize(vmax=abs(_zi).max(), vmin=-abs(_zi).max()))
        plt.colorbar()
        plt.plot(_x, _y, 'ko', ms=3)
        plt._xlim(_x_min, _x_max)
        plt._ylim(_y_min, _y_max)
        plt.title(_title)
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

    def set_y(self, y_min, y_max, npts) :
        if self._x is not None :
            seed(0)
            # this should be a matrix of identical columns
            self._y = uniform(self._y_min, self._y_max, _npts) 
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

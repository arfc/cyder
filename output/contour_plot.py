


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
import pdb

class ContourPlot(object) :
    """
    A class representing a contour plot. It needs data, a_xis labels, a title, 
    etc.
    """
    _x_min = -2 
    """
    The minimum value of the values in the x dimension
    """
    _y_min = -2
    """
    The minimum value of the values in the y dimension
    """
    _x_max = 2
    """
    The maximum value of the values in the x dimension 
    """
    _y_max = 2
    """
    The maximum value of the values in the x dimension 
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

    def __init__(self, 
            x_min = -2, 
            y_min = -2, 
            x_max = 2, 
            y_max = 2, 
            x = None,
            y = None,
            z = None,
            ngridx = 100,
            ngridy = 200,
            npts = 200,
            x_label = 'x',
            y_label = 'y',
            ptitle = 'plottitle',
            fname = 'contour_plot.eps'
            ):
        
        self._x_min = x_min
        self._x_max = x_max
        self._y_min = y_min
        self._y_max = y_max
        self._x=x
        self._y=y
        self._z=z

        if self._x is None :
            self._x = self.set_x()
        if self._y is None :
            self._y = self.set_y()
        if self._z is None : 
            self._z = self.set_z()

        if self._xi is None :
            self._xi = self.set_xi(ngridx)
        if self._yi is None :
            self._yi = self.set_yi(ngridx, ngridy)
        if self._zi is None : 
            self._zi = self.set_zi()

        self._npts = npts

        self._x_label = x_label
        self._y_label = y_label
        self._title = ptitle
        self._filename = fname
        
        #self.grid_data_and_contour() #change subplot defs below
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
        x_min=self._x_min 
        y_min=self._y_min
        x_max=self._x_max 
        y_max=self._y_max

        plt.subplot(211)
        plt.contour(xi, yi, zi, n_labels, linewidths=0.5, colors='k')
        plt.contourf(xi, yi, zi, n_labels, cmap=plt.cm.rainbow,
                     norm=plt.normalize(vmax=abs(zi).max(), vmin=-abs(zi).max()))
        plt.colorbar() # draw colorbar
        plt.plot(x, y, 'ko', ms=3)
        plt.xlim(x_min, x_max)
        plt.ylim(y_min, y_max)
        print ('griddata and contour plotted')

    def plot_tricontour(self):
        # tricontour.
        x=self._x 
        y=self._y 
        z=self._z
        n_labels=self._n_labels
        zi=self._zi
        x_min=self._x_min 
        y_min=self._y_min
        x_max=self._x_max 
        y_max=self._y_max
        title=self._title
        x_label=self._x_label
        y_label=self._y_label

        plt.subplot(111) # change this if you want to plot both
        triang = tri.Triangulation(x, y)
        plt.tricontour(x, y, z, n_labels, linewidths=0.5, colors='k')
        plt.tricontourf(x, y, z, n_labels, cmap=plt.cm.rainbow,
                        norm=plt.normalize(vmax=abs(zi).max(), vmin=-abs(zi).max()))
        plt.colorbar()
        plt.plot(x, y, 'ko', ms=3)
        plt.xlim(x_min, x_max)
        plt.ylim(y_min, y_max)
        plt.xlabel(x_label)
        plt.ylabel(y_label)
        plt.title(title)
        print ('tricontour plotted')

    def save_it(self) :
        savefig(self._filename)
        print ('saved as '+ self._filename)

    def set_x(self) :
        if self._x is None :
            seed(0)
            # this should be a matrix of identical columns?
            self._x = uniform(self._x_min, self._x_max, self._npts) 
        return self._x;

    def set_xi(self, ngridx) :
        if self._xi is None :
            spacing = 10./ngridx
            self._xi = np.linspace(self._x_min-spacing, self._x_max+spacing, ngridx)
        return self._xi

    def set_y(self) :
        if self._y is None :
            # this should be a matrix of identical columns
            self._y = uniform(self._y_min, self._y_max, self._npts) 
        return self._y

    def set_yi(self, ngridx, ngridy) :
        if self._yi is None :
            spacing = 10./ngridx
            self._yi = np.linspace(self._y_min-spacing, self._y_max+spacing, ngridy)
        return self._yi

    def set_z(self) :
        if self._z is None :
            # this should be the mass in whatever component
            self._z = self._x*np.exp(-self._x**2 - self._y**2)  
        return self._z;

    def set_zi(self) :
        if self._zi is None :
            self._zi = griddata(self._x, self._y, self._z, self._xi, self._yi, interp='linear')
        return self._zi

    def set_title(self, npts) :
        self._title = 'tricontour (%d points)' % npts
        return self._title

    def set_filename(self) : 
        self._filename = 'contour_plot.eps'
        return self._filename


import output_tools
import numpy as np
import glob
class ContourData(object) :
    """
    A class that holds a lot of data for the contour plot
    """
    _data = None
    """
    A 3xn numpy ndarray to hold n (x,y,z) data points
    """

    _flist = []
    """
    The list of sqlite files to be queried to fill the db.
    n=the length of this list, unless there are duplicates
    """
    _title = 'real'
    _filename = 'real.eps'
    _x=[]
    _y=[]
    _z=[]
    _x_param = ''
    _y_param = ''
    _x_label = ''
    _y_label = ''
    _z_label = ''
    _comp_id = 4
    _npts = 0


    def __init__(self,
            root='deg',
            xparam = 'degradation',
            xlabel = 'degradation',
            yparam = 'advective_velocity',
            ylabel = 'advective_velocity',
            zlabel = 'massKG',
            ngrid=200,
            title = 'Degradation Rate vs. Advective Velocity',
            filename = 'deg_rate.eps'
            ): 
        self._x_param = xparam
        self._x_label = xlabel
        self._y_param = yparam
        self._y_label = ylabel
        self._z_label = zlabel
        self._title = title
        self._filename = str(filename)
        self._flist = self.collect_filenames(root)
        self.extract_data(self._flist) 
        ContourPlot(
            x_min = min(self._x),
            y_min = min(self._y),
            x_max = max(self._x), 
            y_max = max(self._y), 
            x = self._x,
            y = self._y,
            z = self._z,
            ngridx = ngrid,
            ngridy = ngrid,
            npts = len(self._z),
            x_label = self._x_label,
            y_label = self._y_label,
            ptitle = self._title,
            fname = self._filename
            )
        
    def extract_data(self, flist) : 
        for f in flist :
            self.add_run(f)
     
    def add_run(self, dbname) :
        param_query = output_tools.Query(filename=dbname, queryType='nucparams')
        self._x.append( self.get_x_val(param_query))
        self._y.append( self.get_y_val(param_query))
        param_query.execute()
        vals_query = output_tools.Query(dbname, "contaminants", t0=0, tf=100)
        vals_query.execute()
        self._z.append( self.get_z_val(vals_query))
        return self._x, self._y, self._z

    def get_x_val(self, query) : 
        return query.get_param_val(self._comp_id, self._x_param)

    def get_y_val(self, query) : 
        return query.get_param_val(self._comp_id, self._y_param)

    def get_z_val(self, query) : 
        query.collapse_isos()
        data = query.get_data()
        comp_list=query.get_comp_list()
        mass_slice = data[:,comp_list.index(self._comp_id)]
        return mass_slice.max()

    def collect_filenames(self, root) :
        for name in glob.glob(root+'*.sqlite'):
            self._flist.append(name)
            print(name)
            self._npts += 1
        return self._flist

from argparse import ArgumentParser
def main():
    arg_parser = ArgumentParser(description="Plots 2D data from the"
    " contaminants table, when parameterized by data in the nucparams"
    " table.")
    arg_parser.add_argument("-r", metavar="root", type=str, nargs=1, 
            dest="root", help="This is the name root for the sqlite files to plot.")
    arg_parser.add_argument("-xp", metavar="x_param", type=str, nargs=1, 
            dest="x_param", help="This is the x parameter name in the sqlite database.")
    arg_parser.add_argument("-xl", metavar="x_label", type=str, nargs=1, 
            dest="x_label", help="This is the x parameter name as plotted")
    arg_parser.add_argument("-yp", metavar="y_param", type=str, nargs=1, 
            dest="y_param", help="This is the y parameter name in the sqlite database.")
    arg_parser.add_argument("-yl", metavar="y_label", type=str, nargs=1, 
            dest="y_label", help="This is the y label name as plotted")
    arg_parser.add_argument("-zl", metavar="z_label", type=str, nargs=1, 
            dest="z_label", help="This is the z label name as plotted")
    arg_parser.add_argument("-n", metavar="ngrid", type=int, nargs=1, 
            dest="ngrid", help="This number adjusts the contour grid resolution")
    arg_parser.add_argument("-t", metavar="title", type=str, nargs=1, 
            dest="title", help="This is the title of the plot")
    arg_parser.add_argument("-o", metavar="filename", type=str, nargs=1, 
            dest="filename", help="This is the output filename. Include eps.")
    args=arg_parser.parse_args()
    ContourData(
            root = args.root[0],
            xparam = args.x_param[0],
            xlabel = args.x_label[0],
            yparam = args.y_param[0],
            ylabel = args.y_label[0],
            zlabel = args.z_label[0],
            ngrid = args.ngrid[0],
            title = args.title[0],
            filename = args.filename[0]
            )

if __name__=="__main__" :
    main()

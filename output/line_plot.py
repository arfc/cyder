


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
from collections import OrderedDict
import pdb

class LinearPlot(object) :
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
            npts = 200,
            x_label = 'x',
            y_label = 'y',
            ptitle = 'plottitle',
            fname = 'line_plot.eps'
            ):
        
        self._x_min = x_min
        self._x_max = x_max
        self._y_min = y_min
        self._y_max = y_max
        self._x=x
        self._y=y

        if self._x is None :
            self._x = self.set_x()
        if self._y is None :
            self._y = self.set_y()

        self._npts = npts

        self._x_label = x_label
        self._y_label = y_label
        self._title = ptitle
        self._filename = fname
        
        self.sort_data(self._x, self._y)
        self.lineplot()
        self.save_it()

    def lineplot(self) :
        x=self._x 
        y=self._y 
        n_labels=self._n_labels
        x_label = self._x_label
        y_label = self._y_label
        x_min=self._x_min 
        y_min=self._y_min
        x_max=self._x_max 
        y_max=self._y_max
        title=self._title

        plt.subplot(111)
        plt.plot(x, y, 'ko-')
        plt.xlim(x_min, x_max)
        plt.ylim(y_min, y_max)
        plt.xlabel(x_label)
        plt.ylabel(y_label)
        plt.title(title)
        print ('line plot plotted')

    def sort_data(self, x, y) : 
        data_dict = dict(zip(x,y))
        sorted_dict = OrderedDict(sorted(data_dict.items()))
        self._x = []
        self._x = sorted_dict.keys()
        self._y = []
        self._y = sorted_dict.values()

    def save_it(self) :
        savefig(self._filename)
        print ('saved as '+ self._filename)

    def set_x(self) :
        if self._x is None :
            seed(0)
            # this should be a matrix of identical columns?
            self._x = uniform(self._x_min, self._x_max, self._npts) 
        return self._x;

    def set_y(self) :
        if self._y is None :
            # this should be a matrix of identical columns
            self._y = uniform(self._y_min, self._y_max, self._npts) 
        return self._y

    def set_title(self, npts) :
        self._title = 'tricontour (%d points)' % npts
        return self._title

    def set_filename(self) : 
        self._filename = 'contour_plot.eps'
        return self._filename


import output_tools
import numpy as np
import glob
class LinearData(object) :
    """
    A class that holds data for the linear plot
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
    _x_param = ''
    _x_label = ''
    _y_label = ''
    _comp_id = 4
    _npts = 0


    def __init__(self,
            root='deg',
            xparam = 'ref_sol',
            xlabel = 'Reference Solubility Limit $[kg/m^3]$',
            ylabel = 'massKG',
            title = 'Solubility Limitation',
            filename = 'sol.eps'
            ): 
        self._x_param = xparam
        self._x_label = xlabel
        self._y_label = ylabel
        self._title = title
        self._filename = str(filename)
        self._flist = self.collect_filenames(root)
        self.extract_data(self._flist) 
        LinearPlot(
            x_min = min(self._x),
            y_min = min(self._y),
            x_max = max(self._x), 
            y_max = max(self._y), 
            x = self._x,
            y = self._y,
            npts = len(self._y),
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
        self._x.append( self.get_x_val(param_query) )
        param_query.execute()
        vals_query = output_tools.Query(dbname, "contaminants", t0=0, tf=100)
        vals_query.execute()
        self._y.append( self.get_y_val(vals_query))
        return self._x, self._y

    def get_x_val(self, query) : 
        return query.get_param_val(self._comp_id, self._x_param)

    def get_y_val(self, query) : 
        data = query.get_data()
        collapsed_isos = sum(data,axis=2)
        mass_slice = collapsed_isos[30,query.get_comp_list().index(self._comp_id)]
        return mass_slice

    def collect_filenames(self, root) :
        for name in glob.glob(root+'*.sqlite'):
            self._flist.append(name)
            print(name)
            self._npts += 1
        return self._flist

from argparse import ArgumentParser

    
def main():
    arg_parser = ArgumentParset(description="Plots 1D data from the"
    " contaminants table, when parameterized by data in the nucparams"
    " table.")
    arg_parser.add_argument("-r", metavar="root", type=str, nargs=1, 
            dest="root", help="This is the name root for the sqlite files to plot.")
    arg_parser.add_argument("-xp", metavar="x_param", type=str, nargs=1, 
            dest="x_param", help="This is the x parameter name in the sqlite database.")
    arg_parser.add_argument("-xl", metavar="x_label", type=str, nargs=1, 
            dest="x_label", help="This is the x parameter name as plotted")
    arg_parser.add_argument("-yl", metavar="y_label", type=str, nargs=1, 
            dest="y_label", help="This is the y label name as plotted")
    arg_parser.add_argument("-t", metavar="title", type=str, nargs=1, 
            dest="title", help="This is the title of the plot")
    arg_parser.add_argument("-o", metavar="filename", type=str, nargs=1, 
            dest="filename", help="This is the output filename. Include eps.")
    args=arg_parser.parse_args()
    LinearData(
            args.root,
            args.xparam,
            args.xlabel,
            args.ylabel,
            args.title,
            args.filename
            )

if __name__=="__main__" :
    main()

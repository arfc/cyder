


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
import linear_plot as lp

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
            ngridx = 200,
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

        ax = plt.subplot(111)
        plt.contour(xi, yi, zi, n_labels, linewidths=0.5, colors='k')
        plt.contourf(xi, yi, zi, n_labels, cmap=plt.cm.rainbow,
                     norm=plt.normalize(vmax=abs(zi).max(), vmin=-abs(zi).max()))
        labels = ax.get_xticklabels() 
        for label in labels: 
            label.set_rotation(30) 
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

        ax = plt.subplot(111) # change this if you want to plot both
        triang = tri.Triangulation(x, y)
        plt.tricontour(x, y, z, n_labels, linewidths=0.5, colors='k')
        plt.tricontourf(x, y, z, n_labels, cmap=plt.cm.rainbow,
                        norm=plt.normalize(vmax=abs(zi).max(), vmin=-abs(zi).max()))
        labels = ax.get_xticklabels() 
        for label in labels: 
            label.set_rotation(30) 
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
            spacing = self._x_max/ngridx
            self._xi = np.linspace(self._x_min-spacing, self._x_max+spacing, ngridx)
        return self._xi

    def set_y(self) :
        if self._y is None :
            # this should be a matrix of identical columns
            self._y = uniform(self._y_min, self._y_max, self._npts) 
        return self._y

    def set_yi(self, ngridx, ngridy) :
        if self._yi is None :
            spacing = self._x_max/ngridx
            self._yi = np.linspace(self._y_min-spacing, self._y_max+spacing, ngridy)
        return self._yi

    def set_z(self) :
        if self._z is None :
            # this should be the mass in whatever component
            self._z = self._x*np.exp(-self._x**2 - self._y**2)  
        return self._z;

    def set_zi(self) :
        if self._zi is None :
            self._zi = griddata(self._x, self._y, self._z, self._xi, self._yi, interp='nn')
        return self._zi

    def set_title(self, npts) :
        self._title = 'tricontour (%d points)' % npts
        return self._title

    def set_filename(self) : 
        self._filename = 'contour_plot.eps'
        return self._filename


import output_tools
import sqlite3
import numpy as np
import glob
from types import *
class ThermalPlotData(object) :
    """
    A class that holds a lot of STC data for the contour plot
    """
    _filename = 'stc_data_.sqlite'
    _recipe = None
    _conn = None


    def __init__(self,
            filename = 'stc_data.sqlite'
            ):
        self._filename=filename
        self._recipe = {96243: 10}
        self.main()

    def matid_query(self, a, k, s, r) :
        query = ("SELECT stcdata.mat_id  FROM stcdata WHERE "
        "stcdata.alpha_th == "+str(a)+
        " AND "+
        "stcdata.k_th == "+str(k)+
        " AND "+
        "stcdata.spacing == "+str(s)+
        " AND "+
        "stcdata.r_calc == "+str(r))
        q = self.execute(query)
        mat_id = 0
        for row in q : 
            mat_id = row[0]
        return mat_id

    def stc_query(self, matid, iso) :
        t = []
        stc = []
        table_name="mat"+str(matid)
        query =( "SELECT "+table_name+".time , "+table_name+".stc  FROM "+table_name+
        " WHERE "+table_name+".iso == "+str(iso))
        print(query)
        q = self.execute(query)
        for row in q :
            t.append(row[0])
            stc.append(row[1])
        return t, stc

    def execute(self, query) :
        print(query)
        if self._conn == None :
            self._conn = sqlite3.connect(self._filename)
            print(self._conn.execute("SELECT * FROM stcdata"))
        return self._conn.execute(query)

    def calc_max_temp(self, matid, recipe) :
        tc = [0]*1000
        for iso, kg in recipe.iteritems() :
            t, stc = self.stc_query(matid, iso) 
            tc_i = [i * kg for i in stc] 
            temp = [x+x_i for x,x_i in zip(tc,tc_i)]
            tc = list(temp)
        return max(tc)

    def main(self) :
        a_range = list([float("%.8f" % (0.0000001*x)) for x in 
            [1.,2.,3.,4.,5.,6.,7.,8.,9.,10.]])
        k_range = [0.1,0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0,4.5]
        s_range = [2., 5., 10., 15., 20., 25., 50.]
        r_range = [0.1, 0.25,0.5,1.,2.,5.]

        self.set_up_and_plot(a_range, k_range, s_range[1], r_range[1]) # a vs k
        clf()
        #self.set_up_and_plot(a_range, k_range[1], s_range, r_range[1]) # a vs s
        #clf()
        self.set_up_and_plot(a_range, k_range[1], s_range[1], r_range) # a vs r
        clf()
        self.set_up_and_plot(a_range[1], k_range, s_range, r_range[1]) # k vs s
        clf()
        self.set_up_and_plot(a_range[1], k_range, s_range[1], r_range) # k vs r
        clf()
        self.set_up_and_plot(a_range[1], k_range[1], s_range, r_range) # s vs r
        clf()
        self.set_up_linear(a_range, k_range[1], s_range[1], r_range[1]) # a
        clf()
        self.set_up_linear(a_range[1], k_range, s_range[1], r_range[1]) # k
        clf()
        self.set_up_linear(a_range[1], k_range[1], s_range, r_range[1]) # s
        clf()
        self.set_up_linear(a_range[1], k_range[1], s_range[1], r_range) # r
        clf()

    def set_up_and_plot(self, a, k, s, r):
        title = "Maximum Temperature Sensitivity at "
        inparams = {
                "a" : a, 
                "k" : k, 
                "s" : s, 
                "r" : r}
        x_range = None
        y_range = None
        default = ''
        x_vals=[]
        y_vals=[]
        max_t=[]
        for key, v in inparams.iteritems() :
          if type(v) == ListType and x_range == None :
              x_key = key
              x_range = list(v)
              xmin = min(v)
              xmax = max(v)
              xlabel = self.label(key)
          elif type(v) == ListType and x_range != None :
              y_key = key
              y_range = list(v)
              ymin = min(v)
              ymax = max(v)
              ylabel = self.label(key)
          elif type(v) != ListType :
              title += key+"="+ str(v)+" " 
              default += "_" + key+str(v)
        for x in x_range : 
            for y in y_range :
                inparams[x_key] = x
                inparams[y_key] = y
                x_vals.append(x)
                y_vals.append(y)
                max_t.append(self.calc_max_temp(
                    self.matid_query(inparams["a"], inparams["k"], inparams["s"], inparams["r"]), 
                    self._recipe))

        outfile = x_key + y_key + default + ".eps"

        print(x_vals)
        print(y_vals)
        print(max_t)
        return ContourPlot(
            x_min=xmin,
            y_min=ymin,
            x_max=xmax,
            y_max=ymax,
            x = x_vals, 
            y = y_vals,
            z = max_t,
            ngridx = 200,
            ngridy = 200,
            npts = len(max_t),
            x_label=xlabel,
            y_label=ylabel,
            ptitle = title,
            fname = outfile
            )

    def set_up_linear(self, a, k, s, r):
        title = "Maximum Temperature Sensitivity at "
        inparams = {
                "a" : a, 
                "k" : k, 
                "s" : s, 
                "r" : r}
        x_range = None
        default = ''
        x_vals=[]
        y_vals=[]
        max_t=[]
        for key, v in inparams.iteritems() :
            if type(v) == ListType :
                x_key = key
                x_range = list(v)
                xmin = min(v)
                xmax = max(v)
                xlabel = self.label(key)
            elif type(v) != ListType :
                title += key+"="+ str(v)+" " 
                default += "_" + key+str(v)
        for x in x_range : 
            inparams[x_key] = x
            x_vals.append(x)
            max_t.append(self.calc_max_temp(self.matid_query(inparams["a"], 
                inparams["k"], inparams["s"], inparams["r"]), self._recipe))

        outfile = x_key + default + ".eps"

        print(x_vals)
        print(max_t)
        return lp.LinearPlot(
            x_min=xmin,
            y_min=min(max_t),
            x_max=xmax,
            y_max=max(max_t),
            x = x_vals, 
            y = max_t,
            npts = len(max_t),
            x_label=xlabel,
            y_label= r'Maximum Temperature $[^{\circ}K]$',
            ptitle = title,
            xscale = 'linear',
            yscale = 'log',
            fname = outfile,
            save = True
            )

    def label(self, key) : 
        labels = {
                "a": r'Thermal Diffusivity $\alpha_{th} [m^2/s]$',
                "k": r'Thermal Conductivity $k_{th} [W/m/K]$',
                "s": r'Waste Package Spacing $s$ [m]',
                "r": r'Limiting Radius, $r_{lim}$ [m]'
                }
        return labels[key]


if __name__=="__main__" :
    ThermalPlotData('stc_data.sqlite')

import matplotlib.pyplot as plt
from pylab import *
from collections import OrderedDict

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
    _xscale = 'linear'

    """
    <++>
    """
    _yscale = 'linear'

    """
    <++>
    """
    _filename = "out.eps"

    """
    <++>
    """
    _save = True

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
            xscale = 'linear',
            yscale = 'linear',
            fname = 'line_plot.eps',
            save = True
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
        self._xscale = xscale
        self._yscale = yscale
        self._filename = fname
        self._save = save
        
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
        plt.xscale(self._xscale)
        plt.yscale(self._yscale)
        plt.xlim(x_min, x_max*1.01)
        plt.ylim(y_min, y_max*1.01)
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
        if self._save : 
            savefig(self._filename)
            print ('saved as '+ self._filename)
        else :
            print ('Not saved. Continue working on this plot or save_it. ')

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


import matplotlib
import os
from output_tools import Query
from numpy import cumsum
from file_io import list2file

def resource_file(dbname, receiver):
    outfile = dbname.strip('sqlite') + "txt"
    new_query = Query(dbname, 'resource')
    list2file(outfile, 'resource',
              list(cumsum(new_query.all_received_by(receiver))))


def print_contaminants(dbname):
    new_query = query_contaminants(dbname)
    print new_query.dataLabels
    print new_query.getData()


def query_contaminants(dbname):
    return Query(dbname, 'contaminants', t0=0, tf=100)


def plot_contaminants(dbname, plttype, pltname, stream_dim='IsoID', select_dim=None,
                      select_item=None):
    query = query_contaminants(dbname)
    query.execute()
    if plttype is 'bar' :
        query.bar_plot(streamDim=stream_dim, selectDim=select_dim,
                         selectItem=select_item)
    elif plttype is 'river' :
        query.river_plot(streamDim=stream_dim, selectDim=select_dim,
                         selectItem=select_item)
    else :
        raise Exception("The plottype, " + plttype + ", is not recognized.") 
    query.save_plot(pltname+".eps")
    query.clear_fig()


def plot_all_comps(dbname, plttype, pltroot):
    query = query_contaminants(dbname)
    comps = query.get_comp_list()
    ind = 0
    for comp in comps:
        pltname = pltroot + str(ind) 
        plot_contaminants(dbname, plttype, pltname,
                          stream_dim='IsoID', select_dim='CompID',
                          select_item=comp)
        ind += 1

def stack_comps(dbname, plttype, pltroot):
    query = query_contaminants(dbname)
    plot_contaminants(dbname, plttype, pltroot, stream_dim='CompID', select_dim='IsoID',
                      select_item=92235)

def plot_this_dir() :
    dir_list = os.listdir(".")
    for fname in dir_list :
      if ".sqlite" in fname : 
        plot_all(fname)

def plot_all(dbname) :
    plot_contaminants(dbname, "bar", dbname.replace('.sqlite', ''), 
            stream_dim='CompID', select_dim='IsoID', select_item=92235) 
    plot_all_comps(dbname, "bar", dbname.replace('.sqlite', ''))
    

def plot_contour(root) :




if __name__ == "__main__":
    import sys
    plot_all(str(sys.argv[1]))

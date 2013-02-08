import matplotlib
matplotlib.use('Agg')
from output_tools import Query
from numpy import cumsum
from file_io import list2file

def resource_file(dbname, receiver):
    outfile = dbname.strip('sqlite') + "txt"
    new_query = Query(dbname,'resource')
    list2file(outfile,'resource', list(cumsum(new_query.allReceivedBy(receiver))))

def print_contaminants(dbname):
    new_query = query_contaminants(dbname)
    print new_query.dataLabels
    print new_query.getData()
    
def query_contaminants(dbname) : 
    return Query(dbname,'contaminants',t0=0, tf=10)

def plot_contaminants(dbname, pltname, stream_dim = 'IsoID', select_dim=None, select_item=None) :
    query=query_contaminants(dbname)
    query.execute()
    query.riverPlot(streamDim=stream_dim, selectDim=select_dim, selectItem=select_item)
    query.savePlot(pltname)
    query.clear_fig()

def plot_all_comps(dbname, pltroot): 
    query = query_contaminants(dbname)
    comps = query.getCompList()
    ind = 0
    for comp in comps : 
        pltname=pltroot+str(ind)+".eps"
        plot_contaminants(dbname, pltname, stream_dim='IsoID',select_dim='CompID', select_item=comps.index(comp))
        ind+=1

def stack_comps(dbname, pltroot): 
    query = query_contaminants(dbname)
    pltname=pltroot+".eps"
    plot_contaminants(dbname, pltname, stream_dim='CompID',select_dim='IsoID', select_item=92235)


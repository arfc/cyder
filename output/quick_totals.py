from output_tools import Query
from numpy import cumsum
from file_io import list2file

def resource_file(dbname, receiver):
    outfile = dbname.strip('sqlite') + "txt"
    new_query = Query(dbname,'resource')
    list2file(outfile,'resource', list(cumsum(new_query.allReceivedBy(receiver))))

def contaminant_file(dbname):
    new_query = Query(dbname,'contaminants',t0=0, tf=11)
    data = new_query.getData()
    print data

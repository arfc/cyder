import sqlite3
from sets import Set

class DataRange :
    """This class describes the data ranges for each column"""
    alpha_range=Set([1,2.5, 5., 10., 25., 50.])
    k_range=Set([0.1, 0.25, 0.5, 1., 2.5, 5.])
    spacing_range=Set([2, 5,10, 15, 20, 25, 50])
    r_calc_range=Set([2, 5,10, 15, 20, 25, 50])
    iso_range=Set([96242, 96243, 96244, 96245, 96246,
        95241, 95243, 94238, 94240, 94241, 94242, 55134, 55135, 55137, 38090])
    stc_range=Set([1.])


def create_stc_data(name):
    """ This function creates a fake datatable for testing"""
    filename = name
    conn = sqlite3.connect(filename)
    c = conn.cursor()
    c.execute('''CREATE TABLE STCData (alpha_th real, k_th real, spacing integer, 
          r_calc integer, iso integer, stc real)''')
    for a in DataRange.alpha_range :
        print a
        for k in DataRange.k_range :
            for s in DataRange.spacing_range :
                for r in DataRange.r_calc_range :
                    for i in DataRange.iso_range :
                        for stc in DataRange.stc_range :
                            str_to_execute = "{0},{1},{2},{3},{4},{5}".format(a, 
                                    k, s, r, i, stc)
                            c.execute("INSERT INTO STCData VALUES ("+str_to_execute+")")
    conn.commit()
    conn.close()



if __name__=="__main__" :
  import sys
  create_stc_data("stc_data.sqlite")

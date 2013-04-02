import sqlite3
from sets import Set

class DataRange :
    """This class describes the data ranges for each column"""
    alpha_range=Set([1,2.5, 5., 10., 25., 50.])
    k_range=Set([0.1, 0.25, 0.5, 1., 2.5, 5.])
    spacing_range=Set([2, 5,10, 15, 20, 25, 50])
    r_calc_range=Set([2, 5,10, 15, 20, 25, 50])
    iso_range=Set([96242, 96243, 96244, 96245, 96246,
        95241, 95243, 
        94238, 94240, 94241, 94242, 
        55134, 55135, 55137, 
        38090])
    time_range=Set(range(1,1001)) 
    stc_range=Set([1.])

class STCData:
    def __init__(self, name) :
        self.open_=False
        self.name_=name

    def connection(self):
        if(not self.open_) :
            self.conn_ = sqlite3.connect(self.name_)
            self.open_ = True
        return self.conn_

    def close_connection(self):
        self.conn_.commit()
        self.conn_.close()
        self.open_ = False

    def create_stc_data(self):
        c = self.connection().cursor()
        """ This function creates a fake datatable for testing"""
        c.execute('''CREATE TABLE STCData (mat_id INTEGER PRIMARY KEY, alpha_th 
        REAL, k_th REAL, spacing INTEGER, r_calc INTEGER)''')
        m=0
        for a in DataRange.alpha_range :
            print a
            for k in DataRange.k_range :
                for s in DataRange.spacing_range :
                    for r in DataRange.r_calc_range :
                        m +=1;
                        str_to_execute = "{0},{1},{2},{3},{4}".format(m, a, k, s, r)
                        c.execute("INSERT INTO STCData VALUES ("+str_to_execute+")")
                        self.stc_table(m, a, k, s, r)

        self.close_connection()

    def stc_table(self, m, a, k, s, r):
        c = self.connection().cursor()
        table_name = "mat{0}".format(m)
        c.execute('''CREATE TABLE '''+table_name+''' (mat_id INTEGER, iso INTEGER, time 
                INTEGER, stc REAL, FOREIGN KEY(mat_id) REFERENCES 
                STCData(mat_id))''')
        for i in DataRange.iso_range :
            for t in DataRange.time_range :
                for stc in DataRange.stc_range :
                    str_to_execute = "{0},{1},{2},{3}".format(m, i, t, stc)
                    c.execute("INSERT INTO "+table_name+" VALUES ("+str_to_execute+")")

if __name__=="__main__" :
  import sys
  STCData("stc_data.sqlite").create_stc_data()

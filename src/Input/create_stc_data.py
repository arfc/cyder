import sqlite3
import xls2csv
import os

class DataRange :
    """This class describes the data ranges for each column"""
    _alpha_range=list([1,2,3,4,5,6,7,8,9,10,20,30])
    _k_range=list([0.1, 0.25, 0.5, 1., 2.5, 5.])
    _spacing_range=list([2, 5,10, 15, 20, 25, 50])
    _r_calc_range=list([0.1, 0.25, 0.5, 1., 2., 5.])
    _iso_range=list([ 96243, 96244, 96245, 96246,
        95241, 95243, 
        94238, 94240, 94241, 94242,
        55134, 55135, 55137, 
        38090
        ])
    _time_range=list(range(1,1001)) 
    _stc_range=list([1.])


import numpy as np    
class STCData:
    _mat_array=None
    _name = None
    _ranges = DataRange()
    _fake=True
    _xls_files=None
    _a_dim = []
    _k_dim = []
    _s_dim = []
    _r_dim = []
    _i_dim = [] 
    _t_dim = []

    def __init__(self, name, fake) :
        self.open_=False
        self._name = name
        self._fake = fake

        self._a_dim = self._ranges._alpha_range
        self._k_dim = self._ranges._k_range
        self._s_dim = self._ranges._spacing_range
        self._r_dim = self._ranges._r_calc_range
        self._i_dim = self._ranges._iso_range
        self._t_dim = self._ranges._time_range
        self._mat_array = np.zeros((
            len(self._a_dim), 
            len(self._k_dim), 
            len(self._s_dim), 
            len(self._r_dim)))

        if self._fake == False : 
            self._xls_files=XlsFiles()


    def create_stc_data(self) : 
        self._mat_array = self.make_mat_array()
        self.make_stc_tables(self._mat_array)
        print("success")

    def connection(self):
        if(not self.open_) :
            self.conn_ = sqlite3.connect(self._name)
            self.open_ = True
        return self.conn_

    def close_connection(self):
        self.conn_.commit()
        self.conn_.close()
        self.open_ = False

    def make_mat_array(self) :
        c = self.connection().cursor()
        c.execute('''CREATE TABLE STCData (mat_id INTEGER PRIMARY KEY, alpha_th 
        REAL, k_th REAL, spacing INTEGER, r_calc INTEGER)''')
        m=0
        for a in self._a_dim : 
            print a
            a_idx = self._a_dim.index(a)
            for k in self._k_dim : 
                k_idx = self._k_dim.index(k)
                for s in self._s_dim : 
                    s_idx = self._s_dim.index(s)
                    for r in DataRange._r_calc_range :
                        r_idx = self._r_dim.index(r)
                        m +=1;
                        str_to_execute = "{0},{1},{2},{3},{4}".format(m, a, k, s, r)
                        c.execute("INSERT INTO STCData VALUES ("+str_to_execute+")")
                        self._mat_array[a_idx][k_idx][s_idx][r_idx] = m
        self.close_connection()
        return self._mat_array

    def make_stc_tables(self, mat_array) :
        it = np.nditer(mat_array, flags=['multi_index'])
        while not it.finished:
            m = int(it[0])
            a = self._a_dim[it.multi_index[0]]
            k = self._k_dim[it.multi_index[1]]
            s = self._s_dim[it.multi_index[2]]
            r = self._r_dim[it.multi_index[3]] 
            self.stc_table(m, a, k, s, r)
            it.iternext()


    def stc_table(self, m, a, k, s, r):
        t_list=[]
        c = self.connection().cursor()
        table_name = "mat{0}".format(m)
        c.execute('''CREATE TABLE '''+table_name+''' (mat_id INTEGER, iso INTEGER, time 
                INTEGER, stc REAL, FOREIGN KEY(mat_id) REFERENCES 
                STCData(mat_id))''')
        for i in self._i_dim :
            param_str = ("a="+str(a)+" k="+str(k)+" s="+str(s)+" r="+str(r)+
                    " i="+str(i))
            print param_str
            if self._fake :
                t_list = self._t_dim
                stc_list=[1]*len(self._t_dim)
            else :      
                try : 
                    t_list, stc_list = self._xls_files.get_stc(a, k, s, r, i)
                except ValueError, v :
                    print("A sheet for "+param_str+" does not exist")
            if len(t_list) > 0 :
                out = [self.insert_sql(c, table_name, m, i, t,
                    stc_list[t_list.index(t)]) for t in t_list]
        self.close_connection()

    def insert_sql(self, c, table_name, m, i, t, stc):
        str_to_execute = "{0},{1},{2},{3}".format(m, i, t, stc)
        c.execute("INSERT INTO "+table_name+" VALUES ("+str_to_execute+")")

import glob
import re
import shutil
import csv
import codecs
import _csv
class XlsFiles  :
    _xls_file_list = []
    _xls_file_dict = []
    _csv_file_dict = {}
    _csv_file_list = []
    _r_calc_range = []
    _data_ranges = None

    def __init__(self) :
        self.cleandir()
        self._xls_file_list = self.collect_filenames()
        self._xls_file_dict = self.move_xls_files(self._xls_file_list)
        self._xls_file_list = self._xls_file_dict.keys()
        self._data_ranges = DataRange()
        self._r_calc_range = list(self._data_ranges._r_calc_range)
        self._csv_file_list = self.convert_all_files(self._xls_file_dict)

    def cleandir(self):
        for name in glob.glob('[A-Z][a-z]-[0-9][0-9][0-9]*_[0-9]*.xls'):
            os.remove(name)


    def move_xls_files(self, xls_file_list):
        xls_dict = {}
        for xls in xls_file_list : 
            iso, spacing = self.parse_filename(xls)
            new_name = iso+"_"+spacing+".xls"
            shutil.copyfile(xls, new_name)
            xls_dict[new_name] = [iso, spacing]
        return xls_dict

    def collect_filenames(self) : 
        """Looks for all of the xls's in the path"""
        for name in glob.glob('*xls'):
            self._xls_file_list.append(name)
            print(name)
        return self._xls_file_list

    def parse_filename(self, filename) :
        """for each filename, finds the isotope and spacing"""
        iso = self.parse_iso(filename)
        spacing = self.parse_spacing(filename)
        return iso, spacing

    def parse_iso(self, filename) :
        to_ret = re.sub("Repository Spacing [0-9]* Meters ","",filename)
        return re.sub(".xls", "",  to_ret)

    def parse_spacing(self, filename) :
        meters = re.search("[0-9]* Meters", filename)
        if meters == None :
            raise Exception(filename+"borked")
        else :
            return re.sub(" Meters", "", meters.group(0))

    def convert_all_files(self, xls_file_dict) :
        for xls in xls_file_dict.keys() : 
            self._csv_file_list.append(self.convert_file(xls)) 
        return self._csv_file_list

    def convert_file(self, filename) :
        for r_calc in self._r_calc_range : 
            sheet_name = self.convert_sheet(filename, r_calc)
            iso = self.iso_idx(self._xls_file_dict[filename][0])
            spacing = float(self._xls_file_dict[filename][1])
            self._csv_file_dict[sheet_name] = [iso, spacing, r_calc]
            print(sheet_name)
            print(self._csv_file_dict[sheet_name])
        return sheet_name
    
    def convert_sheet(self, filename, r_calc) :
        n_sheet = self._r_calc_range.index(r_calc)
        outname = re.sub('.xls', '_'+str(r_calc)+'.csv', filename)
        os.system("python xls2csv.py -i "+filename+" -o "+
            outname+" -s "+str(n_sheet)+' -p , ') 
        return outname

    def add_to_file_dict(self, filename, isotope, spacing, r_calc) : 
        """adds filename, isotope, spacing, and r_calc to dictionary"""

    def get_sheet(self, i, s, r):
        sheet = ''
        mat_params = [i,s,r]
        for k, v in self._csv_file_dict.iteritems() :
            if v[:] == mat_params[:] :
                sheet = k
        if sheet == '' :
            raise ValueError('sheet not found')
        return sheet

    def get_stc(self, a, k, s, r, i):
        try :
            sheet = self.get_sheet(i, s, r)
        except ValueError, v :
            raise ValueError('sheet not found')
        t=[]
        stc=[]
        with open(sheet) as f : 
            reader = csv.reader((line.replace('\x00','') for line in f), delimiter=",")
            n_row = 0;
            for row in reader :
                if n_row > 1 and int(float(row[1])) < 1001.0 :
                    t.append(int(float(row[1])))
                    stc.append(float(row[self.alpha_kth_key(a,k)])) 
                n_row+=1
        return t, stc

    def alpha_kth_key(self, a, k) : 
        a_dim = self._data_ranges._alpha_range
        k_dim = self._data_ranges._k_range
        index = a_dim.index(a)*len(k_dim) + k_dim.index(k) + 2 
        return index

    def iso_idx(self, name) :
        idx=0
        for k,v in iso_idx_to_name.iteritems() :
            if re.match(v, name) :
                idx=k
        if idx ==0 :
            raise ValueError('no iso "'+name+'" exists in the list')
        return idx

iso_idx_to_name = {
        38090: "Sr-90",
        55134: "Cs-134",
        55135: "Cs-135",
        55137: "Cs-137",
        94238: "Pu-238",
        94240: "Pu-240",
        94241: "Pu-241",
        94242: "Pu-242",
        95241: "Am-241",
        95243: "Am-243",
        96242: "Cm-242",
        96243: "Cm-243",
        96244: "Cm-244",
        96245: "Cm-245",
        96246: "Cm-246"}

if __name__=="__main__" :
  import sys
  STCData("stc_data.sqlite", False).create_stc_data()

import os
from subprocess import call
import fileinput, sys
import re
from optparse import OptionParser
import numpy as np


def make_dir(dir_path) :
    """ makes a directory at path/name/"""
    dpath, dname = os.path.splitext(dir_path)
    directory = full_path(dpath, dname)
    if not os.path.exists(directory):
        os.makedirs(directory)

def configure_infile(xml_in, xml, param, val) :
    """ copies the in_file to out_file, then changes param to val """
    f_old = open(xml_in, 'r') 
    f_new = open(xml, 'w') 
    old = "<"+param+">[^<]*</"+param+">"
    new = "<"+param+">"+str(val)+"</"+param+">"
    for line in f_old :
        f_new.write(re.sub(old, new, line))
    f_old.close()
    f_new.close()
    return xml
     
def configure_infiles(xml_in, param, val_list) :
    """ takes a list of values, each of which will result in runnable xml. """
    xml_list = []
    for val in val_list:
        xml = change_extension(xml_in, str(val)+".xml")
        xml_list.append(configure_infile(xml_in, xml, param, val))
    return xml_list

def run_cyclus(in_file_list, out_path) :
    """ for each infile in the dict, run cyclus and create the outfile in the dict"""
    for in_file in in_file_list : 
        out_file = full_path(out_path, strip_xml(in_file)+".sqlite")
        call(["/usr/local/cyclus/bin/cyclus", in_file, "-o", out_file])

def strip_xml(full_path) :
    """ returns root of the filename. /usr/local/cyclus.xml returns cyclus """
    fpath, fname = os.path.split(full_path)
    return re.sub('.xml$', '', fname)

def strip_in(full_path) :
    """ returns root of the filename. /usr/local/cyclus.xml.in returns cyclus.xml """
    fpath, fname = os.path.split(full_path)
    return re.sub('.in$', '', fname)

def full_path(fpath, fname) :
    """ for a path and a name, create a full path."""
    to_ret = os.path.abspath(os.path.join(fpath, fname))
    return to_ret

def change_extension(fname, new_ext) :
    """ returns filename or full path with new extension """
    root_xml = strip_in(fname)
    root = strip_xml(root_xml)
    return root+new_ext

def make_param_range(low, upper, number):
    """ makes a range between low and high with number vals"""
    arr = np.linspace(float(low), float(upper), int(number))
    return arr.tolist()

def perturb(xml_in, out_path, param, val_list) :
    make_dir(out_path)
    in_file_list = configure_infiles(xml_in, param, val_list)
    run_cyclus(in_file_list, out_path)

def main() :
    usage = "usage: %perturb.py -i foo.xml.in -o ./foo -p ref_sol_lim -l 0 -h 1 -s 0.001 "
    parser = OptionParser(usage)
    parser.add_option("-i", "--xml_in", dest="xml_in",
                      help="read data from foo.xml.in")
    parser.add_option("-o", "--out_path", dest="out_path",
                      help="place sqlite files in out_path")
    parser.add_option("-p", "--param", dest="param",
                      help="the parameter to perturb")
    parser.add_option("-l", "--low", dest="low",
                      help="the low value of the param range")
    parser.add_option("-u", "--upper", dest="upper",
                      help="the upper value of the param range")
    parser.add_option("-n", "--num", dest="number",
                      help="number of values of the param range")
    (o, args) = parser.parse_args()
    param_range = make_param_range(o.low, o.upper, o.number)
    perturb(o.xml_in, o.out_path, o.param, param_range)


if __name__ == "__main__":
    main()

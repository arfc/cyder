import os
import output/quick_totals

def plot_all(in_path, out_path=".") : 
    if ".sqlite" in in_path :
        plot_db(in_path, out_path)
    else :
        plot_this_dir(out_path)

def plot_db(dbname, out_path) :
    if ".sqlite" in in_path :
        quick_totals.plot_all(in_path)



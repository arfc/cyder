import os
from os import path
from subprocess import call

def run_this_dir(in_path, out_path) :
    dir_list = os.listdir(in_path)

    for fname in dir_list : 
        if ".xml" in fname :  
            in_file = path.join(in_path, fname)
            out_file = path.join(out_path, fname.replace(".xml", ".sqlite"))
            call(["/usr/local/cyclus/bin/cyclus", in_file, "-o", out_file])

if __name__ == "__main__":
    import sys
    run_this_dir(str(sys.argv[1]), str(sys.argv[2]))

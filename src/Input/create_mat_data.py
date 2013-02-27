import sqlite3


def create_mat_data(name):
  filename = name
  conn = sqlite3.connect(filename)
  c = conn.cursor()
  c.execute('''CREATE TABLE clay (elem integer, d real, k_d real, s real)''')
  elements = range(1,120)
  for elem in elements :
    d = elem*10.**(-6)
    k_d = elem/200.
    s = elem*2.
    str_to_execute = "{0},{1},{2},{3}".format(elem,d,k_d,s)
    c.execute("INSERT INTO clay VALUES ("+str_to_execute+")")
  conn.commit()
  conn.close()

if __name__=="__main__" :
  import sys
  create_mat_data("mat_data.sqlite")

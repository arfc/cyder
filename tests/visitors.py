from __future__ import print_function

import re

_table_names = {"agents": "Agents",}

_agent_key = "ID"
_agent_schema = ["AgentType", "ModelType", "Prototype", "ParentID", "EnterDate"]

_agent_id_names = ["ParentID"]

class HDF5RegressionVisitor(object):
    """ An HDF5RegressionVisitor visits a number of Cyclus HDF5 tables,
    returning objects that can be equality-comparable with other visitors.
    """

    def __init__(self, db):
        """Parameters
        ----------
        db : PyTables File
           The database associated with this visitor
        """
        self._db = db
        self.agent_invariants = self._populate_agent_invariants()

    def _populate_agent_invariants(self):
        invars = {}
        table = self._db.get_node(self._db.root,
                                  name = _table_names["agents"], 
                                  classname = "Table")
        for row in table.iterrows():
            a_id = row["ID"]
            p_id = row["ParentID"]
            p_invar = None
            if (a_id != p_id):
                p_invar = invars[p_id]
            invars[a_id] = tuple(row[i] if i not in _agent_id_names else p_invar 
                                 for i in _agent_schema)
        return invars
    
    def walk(self):
        """Visits all tables, populating an equality-comparable object
        """
        ret = set()
        for table in self._db.walk_nodes(classname = "Table"):
            methname = 'visit' + re.sub('([A-Z]+)', r'_\1', table._v_name).lower()
            # print(methname)
            if hasattr(self, methname):
                meth = getattr(self, methname)
                obj = meth(table)
                ret.add(obj)
        return ret

    def visit_agents(self, table):
        return tuple(
            tuple(row[i] 
                  if i not in _agent_id_names or i is _agent_key
                  else self.agent_invariants[row[i]] 
                  for i in _agent_schema
                  )
            for row in table.iterrows())

# query.py
import sqlite3
import numpy as np
from numpy import zeros
from numpy import cumsum
from numpy import mean
from matplotlib import cm as cm
import pylab

###############################################################################
###############################################################################


class Query(object):
    """
    A class representing a Query on a Cyclus output database.
    """

    q_type = ''
    """
    The Query type. Right now we support 'resource'
    """

    q_stmt = None
    """
    The sql_stmt representing this Query.
    """

    conn = None
    """
    The connection to the database.
    """

    data = None
    """
    The array that holds that data itself.
    """

    data_axes = []
    """
    An ordered list of the names of each of the dimenions in the data array.
    """

    data_units = []
    """
    An ordered list of the units on each of the dimenions in the data array.
    """

    data_labels = [0] * 4
    """
    An ordered list of lists of the labels for each discrete item on each axis.
    """

    is_executed = False
    """
    True if this Query's SQL statement has been executed and the array contains
    real data.
    """

    is_plottable = False
    """
    True if this Query's data is currently plottable, whatever that will
    eventually mean.
    """

    t0 = 0
    """
    The initial time over which this Query is operating.
    """

    tf = 1200
    """
    The final time over which this Query is operating.
    """

    iso_to_ind = {}
    """
    A mapping of codes to indices for the isotope dimension.
    """

    ind_to_iso = {}
    """
    A mapping of indices to codes for the isotope dimension.
    """

    ax = None
    """
    Stores the axes formed when plotting the data in this Query.
    """

    legend=None
    """
    Stores the legend formed if there is one.

    """

    figure = None
    """
    Stores the figure formed by plotting the data in this Query.
    """

    param_ind_to_name = {}
    """
    A mapping of indices to param names for the param dimension
    """
    param_name_to_ind = {}
    """
    A mapping of param names to indices for the param dimension
    """

###############################################################################

    def __init__(self, filename, queryType, t0=0, tf=1200):
        """
        Creates the Query object by connecting to the database, checking that
        the Query type specified is valid, and initializing a partial SQL
        statment querying the correct quantity over the correct time period.
        Like the python range() function, the rule for the time bounds is that
        the lower is in-clusive and the upper, ex-clusive
        """

        # Save the times.
        self.t0 = t0
        self.tf = tf

        # Check type.
        q_types = ['material', 'resource', 'contaminants', 'nucparams']
        if (queryType in q_types):
            self.q_type = queryType
        else:
            raise QueryException("Error: " + queryType +
                                 " is not a recognized Query type.")

        # Initialize the SQL.
        if 'material' == queryType:
            self.set_q_stmt(sql_stmt("Transactions.Time, " +
                                     "Transactions.senderID, " +
                                     "Transactions.receiverID, " +
                                     "IsotopicStates.IsoID, "
                                     "IsotopicStates.value ",
                                     "Transactions, IsotopicStates",
                                     "Transactions.Time >= " + str(t0) +
                                     " AND " + "Transactions.Time < " +
                                     str(tf)))
        elif 'resource' == queryType:
            self.set_q_stmt(
                sql_stmt("Transactions.Time, Transactions.senderID, " +
                         "Transactions.receiverID, " +
                         "TransactedResources.Quantity",
                         "Transactions, TransactedResources",
                         "Transactions.ID == " +
                         "TransactedResources.TransactionID AND " +
                         "Transactions.Time >= " + str(t0) + " AND " +
                         "Transactions.Time < " + str(tf)))
        elif 'contaminants' == queryType:
            self.set_q_stmt(sql_stmt("contaminants.Time, " +
                                     "contaminants.CompID, " +
                                     "contaminants.IsoID, " +
                                     "contaminants.MassKG",
                                     "contaminants",
                                     "contaminants.Time >= " +
                                     str(t0) + " AND " +
                                     "contaminants.Time < " +
                                     str(tf)))
        elif 'nucparams' == queryType:
            self.set_q_stmt(sql_stmt("nuclidemodelparams.CompID, " +
                                     "nuclidemodelparams.param_name, " +
                                     "nuclidemodelparams.param_val",
                                     "nuclidemodelparams"))


        self.conn = sqlite3.connect(filename)

        # Record the labels and the (default) units for the Query.
        if self.q_type == 'material':
            self.data_axes = ['time', 'from', 'to', 'iso']
            self.data_units = ['months', 'agentID', 'agentID', 'tons']
            # Generate isotope maps.
            isos = ()
            for index, iso in enumerate(isos):
                self.iso_to_ind[iso] = index
                self.ind_to_iso[index] = iso
        elif self.q_type == 'resource':
            self.data_axes = ['time', 'from', 'to']
            self.data_units = ['months', 'agentID', 'agentID']
            # Generate isotope maps.
            isos = ()
            for index, iso in enumerate(isos):
                self.iso_to_ind[iso] = index
                self.ind_to_iso[index] = iso
        elif self.q_type == 'contaminants':
            self.data_axes = ['time', 'CompID', 'IsoID', 'MassKG']
            self.data_units = ['months', 'CompID', 'IsoID', "kg"]
        elif self.q_type == 'nucparams':
            self.data_axes = ['CompID', 'param_name', 'param_val']
            self.data_units = ['CompID', 'name', 'val']

###############################################################################
    def set_q_stmt(self, q_stmt):
        """
        Sets the query statement to whatever query statement is passed in
        """
        self.q_stmt = q_stmt

###############################################################################
    def all_received_by(self, recID):
        """
        This filters the data to include only the material or resources
        received by the model with ID = recID
        """
        try:
            toDim = self.data_axes.index('to')
        except ValueError:
            print "Warning: Query data no longer have a 'to' dimension."
            return

        if 'resource' == self.q_type:

            # get the list of actors
            actList = self.get_act_list()
            numActs = len(actList)

            # Initialize the list.
            totRsrc = zeros((self.tf - self.t0) / 12.)

            # Perform the SQL query.
            c = self.conn.cursor()
            c.execute("SELECT Transactions.Time, " +
                      "TransactedResources.Quantity " +
                      "FROM Transactions, TransactedResources " +
                      "WHERE transactions.ID == " +
                      "transactedresources.transactionID AND " +
                      "transactions.receiverID == ? ", (recID,))

            # Load the results into the array.
            timeInd = -1
            for row in c:
                time = row[0] - self.t0
                if (time % 12 - 1 == 0):
                    quan = row[1]
                    totRsrc[timeInd] += quan
                    timeInd += 1
            return totRsrc

        else:
            raise QueryException("Error: " + queryType +
                                 "cannot return all resources received.")

###############################################################################
    def collapse_isos_to_elts(self, EltsList=[92]):
        """
        Input:
        -EltsList: Elements that have to be read. For instance, EltsList =
        ['92'] will sum all U isotopes, elt = ['92235'] will only return the
        U235 mass in the material, EltsList = ['92235', '92238'] will sum U235
        and U238 masses.

        Returns:
        -the total mass (in tons) of asked elements/isotopes.
        """

        isoDim = self.data_axes.index('iso')
        L = self.data[isoDim]
        TotU = 0

        # Loop through various isotops of the material
        for i in range(1, int(L[0]) * 2, 2):
            L[i] = int(L[i])
            iso = L[i] / 10
            N = iso % 1000
            P = iso / 1000

            # Only requested isotops or elements are read
            if iso in EltsList or P in EltsList or 'All' in EltsList:

                # The mass of that uranium isotope is added to the total mass
                # of U of the shipment. The density number (nb of atoms) is
                # converted into tons of Uranium
                TotU += L[i + 1] * N / 6.02214e29

        return round(TotU, 3)

###############################################################################

    def collapse_isos(self):
        """
        Collapses the isotope set for this Query from whatever it currently is
        to a single mass value, eliminating an entire dimension in the array
        representation. This function may only be called after execution.
        """

        if not self.is_executed:
            raise QueryException("Error: operations on the isotope " +
                                 "dimension can be performed only " +
                                 "after Query execution.")

        try:
            isoDim = self.data_axes.index('iso')
        except ValueError:
            print "Warning: Query data no longer have an 'isotope' dimension."
            return

        self.data = sum(self.data, isoDim)
        self.data_axes.pop(isoDim)
        self.data_labels.pop(isoDim)
        self.data_units.pop(isoDim)

###############################################################################
    def collapse_senders(self):
        """
        Collapses the 'from' set for this Query from whatever it currently is
        to a single, summed value, which is to say we sum over and eliminate
        the 'from' dimension. Thus, this function can only be called after
        execution.
        """
        if not self.is_executed:
            raise QueryException("Error: operations on the 'from' dimension " +
                                 "can be performed only after Query " +
                                 "execution. If you want to limit the " +
                                 "number of total actors, use the " +
                                 "appropriate collapseActorsTo...() function.")

        try:
            fromDim = self.data_axes.index('from')
        except ValueError:
            print "Warning: Query data no longer has a 'from' dimension."
            return

        self.data = sum(self.data, fromDim)
        self.data_axes.pop(fromDim)
        self.data_labels.pop(fromDim)
        self.data_units.pop(fromDim)

###############################################################################

    def collapse_receivers(self):
        """
        Collapses the 'to' set for this Query from whatever it currently is
        to a single, summed value, which is to say we sum over and eliminate
        the 'to' dimension. Thus, this function can only be called after
        execution.
        """
        if not self.is_executed:
            raise QueryException("Error: operations on the 'to' dimension " +
                                 "can be performed only after Query " +
                                 "execution. If you want to limit the " +
                                 "number of total actors, use the " +
                                 "appropriate collapseActorsTo...() function.")

        try:
            toDim = self.data_axes.index('to')
        except ValueError:
            print "Warning: Query data no longer have a 'to' dimension."
            return

        self.data = sum(self.data, toDim)
        self.data_axes.pop(toDim)
        self.data_labels.pop(toDim)
        self.data_units.pop(toDim)

###############################################################################

    def collapse_senders_and_receivers(self):
        """
        Performs a signed material flow analysis. Subtracts the array summed
        over 'from' dimension from the array summed over the 'to' dimension.
        The resulting array contains signed material transfer information
        suitable for plots where we want to study where material resided at a
        particular time. This function can only be called after execution.
        """
        if not self.is_executed:
            raise QueryException("Error: operations on the 'from' and 'to' " +
                                 "dimensions can be performed only after " +
                                 "Query execution. If you want to limit " +
                                 "the number of total actors, use the " +
                                 "appropriate collapseActorsTo...() " +
                                 "function.")
        try:
            fromDim = self.data_axes.index('from')
            toDim = self.data_axes.index('to')
        except ValueError:
            print("Warning: Query data no longer have both a 'from' and 'to' \
                    dimension.")
            return

        self.data = sum(self.data, fromDim) - sum(self.data, toDim)

        self.data_axes.pop(fromDim)
        oldLabels = self.data_labels.pop(fromDim)
        oldUnits = self.data_units.pop(fromDim)

        self.data_axes.insert(fromDim, 'thru')
        self.data_labels.insert(fromDim, oldLabels)
        self.data_units.insert(fromDim, oldUnits)

        self.data_axes.pop(toDim)
        self.data_labels.pop(toDim)
        self.data_units.pop(toDim)

###############################################################################

    def integrate_over_time(self):
        """
        Integrates the results of this Query over the time dimension (like
        turning a PDF into a CDF). Especially appropriate for creating
        river plots. This function must be called after execution.
        """

        if not self.is_executed:
            raise QueryException("Error: integration over time can only be " +
                                 "performed after Query execution.")

        try:
            timeDim = self.data_axes.index('time')
        except ValueError:
            print "Warning: Query data no longer have both a 'time' dimension."
            return

        self.data = cumsum(self.data, timeDim)

###############################################################################
    def get_data(self):
        """
        Executes the current form of the Query if it has not been executed
        already
        and returns the numpy array that stores the data.
        """

        # If we haven't executed, execute.
        if not self.is_executed:
            self.execute()

        # Return the array.
        return self.data

###############################################################################
    def get_act_list(self):
        """
        Count and record how many actors exist during the range of the
        calculation.
        """
        c = self.conn.cursor()

        actList = []
        c.execute("SELECT Agents.ID FROM Agents, Transactions " +
                  "WHERE Agents.EnterDate + Agents.LeaveDate > ? " +
                  "AND Agents.EnterDate <= ? AND " +
                  "(Agents.ID = Transactions.SenderID OR " +
                  "Agents.ID = Transactions.ReceiverID) ", (self.t0, self.tf))

        for row in c:
            if row[0] not in actList:
                actList.append(row[0])

        actList.sort()
        return actList

###############################################################################
    def get_comp_names(self):
        """
        Count and record how many Components exist during the range of the
        calculation.
        """
        c = self.conn.cursor()

        compTypes = {}
        c.execute("SELECT components.CompID, " +
                  "components.name FROM components")

        for row in c:
            if row[0] not in compTypes:
                compTypes[row[0]] = row[1]

        return compTypes

###############################################################################
    def get_comp_list(self, table='contaminants'):
        """
        Count and record how many Components exist during the range of the
        calculation.
        """
        c = self.conn.cursor()

        compList = []
        c.execute(
            "SELECT components.CompID FROM components, " +
            table)

        for row in c:
            if row[0] not in compList:
                compList.append(row[0])

        compList.sort()
        return compList

###############################################################################
    def get_param_val(self, compID, param_name):
        """
        Gets the parameter value of the parameter specified in the CompID component 
        """

        c = self.conn.cursor()

        c.execute("SELECT NuclideModelParams.ParamVal " +
            "FROM NuclideModelParams "+
            "WHERE NuclideModelParams.CompID=" + str(compID) + " " +
            "AND NuclideModelParams.ParamName='" + str(param_name) + "' ")

        for row in c : 
            param_val = row[0]
        return param_val

###############################################################################
    def get_short_params_list(self, table='nuclidemodelparams'):
        """
        Count and record how many IsoIDs exist in the table, and make a list
        """
        c = self.conn.cursor()

        param_list = []
        c.execute("SELECT " + table + ".param_name FROM " + table)

        for row in c:
            if row[0] not in param_list:
                param_list.append(row[0])

        param_list.sort()
        return param_list

###############################################################################
    def get_short_iso_list(self, table='contaminants'):
        """
        Count and record how many IsoIDs exist in the table, and make a list
        """
        c = self.conn.cursor()

        iso_list = []
        c.execute("SELECT " + table + ".IsoID FROM " + table)

        for row in c:
            if row[0] not in iso_list:
                iso_list.append(row[0])

        iso_list.sort()
        return iso_list

###############################################################################
    def execute(self):
        """
        Executes the current form of the Query, storing the data in a numpy
        array.
        If you want to execute AND return the data, you can use get_data().
        """

        # If we've already executed, report as such with an Exception. This may
        # be an important warning. Perhaps we should implement a reExecute for
        # cases where we want the Query to read from the database again rather
        # than
        # simply create a new Query object.
        if self.is_executed:
            raise QueryException("Error: This query has already been " +
                                 "executed. Try reExecute().")

        c = self.conn.cursor()

        if 'material' == self.q_type:

            # Get the array dimensions. We don't know if we've filtered or
            # collapsed
            # away some of
            # the potential result space, so we need to assume the array has
            # the
            # following dimensions (and size).
            # time (tf - t0) X from (numFrom) X to (numTo) X iso (numIsos)
            # or time X from X to

            # get the list of actors
            actList = self.get_act_list()
            numActs = len(actlist)

            # Get the list of isotopes from the hard-coded list in . Count
            # them up and make a dictionary for mapping them into the iso
            # dimension
            # of the Query's data array.
            numIsos = len(self.ind_to_iso)

            # Initialize the array.
            try:
                self.data = zeros(
                    (self.tf - self.t0, numActs, numActs, numIsos))
            except ValueError:
                raise QueryException("Error: you've executed a Query whose " +
                                     "array representation would be " +
                                     str(self.tf - self.t0) + " x " +
                                     str(numActs) + " x " + str(numActs) +
                                     " x " + str(numIsos) +
                                     ". That's too large.")

            # Perform the SQL query.
            c.execute(str(self.q_stmt))

            # Load the results into the array.
            fromInd = -1
            toInd = -1
            for row in c:
                time = row[0] - self.t0
                fFac = row[1]
                tFac = row[2]
                nIso = row[3]
                mIso = row[4]

                # Get the indexes for the 'from' and 'to' dimensions.
                d = self.conn.cursor()
                d.execute(
                    "SELECT Agents.ID FROM Agents WHERE Agents.ID = ? ",
                    (fFac,))

                for roe in d:
                    fromInd = actList.index(roe[0])

                d.execute("SELECT Agents.ID FROM Agents " +
                          "WHERE Agents.ID = ? ", (tFac,))

                for roe in d:
                    toInd = actList.index(roe[0])

                self.data[time][fromInd][toInd][self.iso_to_ind[nIso]] += mIso

            # Store the labels.
            self.data_labels[0] = range(self.t0, self.tf)
            self.data_labels[1] = actList
            self.data_labels[2] = actList
            self.data_labels[3] = self.ind_to_iso.values()

        elif 'resource' == self.q_type:

            # Get the array dimensions. We don't know if we've filtered or
            # collapsed
            # away some of
            # the potential result space, so we need to assume the array has
            # the
            # following dimensions (and size).
            # time (tf - t0) X from (numFrom) X to (numTo) X iso (numIsos)
            # or time X from X to

            # get the list of actors
            actList = self.get_act_list()
            numActs = len(actList)

            # Initialize the array.
            try:
                self.data = zeros((self.tf - self.t0, numActs, numActs))
            except ValueError:
                raise QueryException("Error: you've executed a Query whose " +
                                     "array representation would be " +
                                     str(self.tf - self.t0) + " x " +
                                     str(numActs) + " x " + str(numActs) +
                                     ". That's too large.")

            # Perform the SQL query.
            c.execute(str(self.q_stmt))

            # Load the results into the array.
            fromInd = -1
            toInd = -1
            for row in c:
                time = row[0] - self.t0
                fFac = row[1]
                tFac = row[2]
                rsrc = row[3]

                # Get the indexes for the 'from' and 'to' dimensions.
                d = self.conn.cursor()
                d.execute(
                    "SELECT Agents.ID FROM Agents WHERE Agents.ID = ? ",
                    (fFac,))

                for roe in d:
                    fromInd = actList.index(roe[0])

                d.execute("SELECT Agents.ID FROM Agents " +
                          "WHERE Agents.ID = ? ", (tFac,))

                for roe in d:
                    toInd = actList.index(roe[0])

                self.data[time][fromInd][toInd] += rsrc

            # Store the labels.
            self.data_labels[0] = range(self.t0, self.tf)
            self.data_labels[1] = actList
            self.data_labels[2] = actList

        elif 'contaminants' == self.q_type:
            # Get the array dimensions. We don't know if we've filtered or
            # collapsed
            # away some of
            # the potential result space, so we need to assume the array has
            # the
            # following dimensions (and size).
            # time (tf - t0) X iso (numIsos) X components

            # get the list of actors
            # Perform the SQL query.
            actList = self.get_comp_list()
            numActs = len(actList)
            isos = self.get_short_iso_list('contaminants')
            for index, iso in enumerate(isos):
                self.iso_to_ind[iso] = index
                self.ind_to_iso[index] = iso
            numIsos = len(self.ind_to_iso)

            # Initialize the array.
            try:
                self.data = zeros((self.tf - self.t0, numActs, numIsos))
            except ValueError:
                raise QueryException("Error: you've executed a Query whose " +
                                     "array representation would be " +
                                     str(self.tf - self.t0) + " x " +
                                     str(numActs) + " x " + str(numIsos) +
                                     ". That's too large.")

            c.execute(str(self.q_stmt))

            # Load the results into the array.
            compInd = -1
            for row in c:
                time = row[0] - self.t0
                comp = row[1]
                iso = row[2]
                mass = row[3]

                compInd = actList.index(comp)

                self.data[time][compInd][self.iso_to_ind[iso]] += mass

            # Store the labels.
            self.data_labels[0] = range(self.t0, self.tf)
            self.data_labels[1] = actList
            self.data_labels[2] = self.ind_to_iso.values()

        elif 'nucparams' == self.q_type:
            actList = self.get_comp_list('nuclidemodelparams')
            numActs = len(actList)
            params = self.get_short_param_list('nuclidemodelparams')
            for index, param in enumerate(params):
                self.param_ind_to_name[index] = param
                self.param_name_to_ind[param] = index
            numParams = len(params)

            # Initialize the array.
            try:
                self.data = zeros((numActs, numParams))
            except ValueError:
                raise QueryException("Error: you've executed a Query whose " +
                                     "array representation would be " +
                                     str(numActs) + " x " +
                                     str(numParams) + ". That's too large.")


            c.execute(str(self.q_stmt))
            compInd = -1
            for row in c : 
              comp = row[0]
              param_name = row[1]
              param_val = row[2]

              compInd = actList.index(comp)
              paramInd = param_name_to_ind[param]
              self.data[compInd][paramInd] = param_val

        self.is_executed = True

###############################################################################
    def check_plottable(self, streamDim=None, streamList=None,
                        selectDim=None):

        if not self.is_executed:
            raise QueryException("Error: plotting can only be " +
                                 "performed after Query execution.")

        if selectDim == streamDim:
            raise QueryException("Error, streamDim and selectDim values " +
                                 "were the same. To plot only a single " +
                                 "stream when streamDim has more than one " +
                                 "element, use a single-item streamList.")

        # Parse the dimensions.
        try :
            timeDim = self.data_axes.index('time')
            streamDim = self.data_axes.index(streamDim)
            if selectDim != None:
                selectDim = self.data_axes.index(selectDim)
        except ValueError:
            raise QueryException("Warning: Query data no longer have the " +
                                 "requested dimension (and the 'time' " +
                                 "dimension, without which it's not " +
                                 "meaningful to create a river plot.")

        # Make sure the we don't have too many data dimensions.
        if len(self.data.shape) > 3:
            raise QueryException("Warning: data dimensionality too large. " +
                                 "You can't do a river plot of data that's " +
                                 "larger than 3D--two dimensions that you " +
                                 "plot and one dimenion that you choose an " +
                                 "item from.")

        # If they gave no streamlist, assume they want all possible streams.
        if None == streamList:
            streamList = self.data_labels[streamDim]

        return timeDim, streamDim, selectDim, streamList

###############################################################################
    def prep_data(self, stream_dim=None, select_dim=None, select_item=None):

        # Let's create a new view of the data to plot...
        plot_data = self.data

        # And reduce it if that's what we've been told to do.
        if select_dim is not None :
            if select_item is None :
                raise QueryException("If you specify a select_dim, you must " +
                                     "specify the label of the item you " +
                                     "want to select.")
            select_ind = self.data_labels[select_dim].index(select_item)
            if 1 == select_dim and 2 == stream_dim:
                plot_data = plot_data[:, select_ind, :]
            elif 2 == select_dim and 1 == stream_dim:
                plot_data = plot_data[:, :, select_ind]
            else:
                raise QueryException("Error: bad function input or the " +
                                     "data axes have gotten out of order " +
                                     "somehow.")
        # Now we should be down to two dimensions. Check.
        if len(plot_data.shape) != 2:
            raise QueryException("Error: bad stream_dim/select_dim combo. "
                                 "You can only make a river plot of a 2D " +
                                 "data array.")
        return plot_data


###############################################################################
    def set_up_figure(self):
        # Create the figure and the data we need to do the plotting.
        self.figure = pylab.figure(1)  # the figure
        self.ax = self.figure.add_subplot(111)  # the axes

###############################################################################
    def bar_plot(self, streamDim=None, streamList=None,
                 selectDim=None, selectItem=None):
        """
        Creates a stacked bar histogram plot of the data in Query.
        'time' and at least one (but no more than two) other axes must exist.

        Plots the data in the dimension 'streamDim' against time. If a
        streamList of label names of items in the streamDim axis is given, we
        plot only those streams instead of all of them. If the data array is
        currently three dimensional, the user must also specify a dimension,
        selectDim, and a label of an element in that dimension, selectItem, to
        plot.

        3D example:

        To make an isotope-wise bar plot for facility 5 when the array
        looks like this:

        data_axes = ['time', 'thru', 'iso']
        data_units = ['months', 'facID', 'tons']

        call

        q.bar_plot(streamDim = 'iso', selectDim = 'thru', selectItem = 5)
        """
        time_dim, stream_dim, select_dim, stream_list = \
                self.check_plottable(streamDim, streamList, selectDim) 

        plot_data = self.prep_data(stream_dim, select_dim, selectItem)

        self.set_up_figure()


        # For RANDOM colors:
        #colors = pylab.rand(len(stream_list),len(stream_list))
        
        # get time dimension labels
        t = self.data_labels[time_dim]  
        run_sum = zeros(self.data.shape[time_dim])

        # Turn the list of stream labels into a list of indices.
        indList = [0] * len(stream_list)
        for i, s in enumerate(stream_list):
            indList[i] = self.data_labels[stream_dim].index(s)

        # get labels for the stream dimension
        stream_labels = {}
        if streamDim == 'CompID':
            # get the component types
            comp_names = self.get_comp_names()
            for comp_id, comp_name in comp_names.iteritems() : 
                # "WF3"
                stream_labels[comp_id] = comp_name + str(comp_id)
            self.ax.set_title(self.data_axes[select_dim] + " = " + 
                    str(selectItem))
        elif streamDim == 'IsoID':
            # get the isotope names
            # "92235"
            for i in self.data_labels[stream_dim] :
              stream_labels[i] = str(i)
            self.ax.set_title(self.data_axes[select_dim] + " = " + 
                    self.get_comp_names()[selectItem] + str(selectItem))
        else : 
            raise QueryException(
                    "Error: Bar Plots are currently supported only for CompID \
                    and IsoID streamDims.")

            
        # Iterate through the streams and add them to the plot.
        legend_items=[]
        legend_ids=[]
        for ind in indList:
          if(max(plot_data[:, ind] > 0)):
            for time in t :
              time-=self.t0
              the_plot = self.ax.bar(time,
                                     plot_data[time, ind], 
                                     width=1,
                                     bottom=run_sum[time], 
                                     color=cm.jet(float(ind)/float(len(stream_list)), 
                                         alpha=0.5), 
                                     label=ind)
              run_sum[time] += plot_data[time, ind]
            legend_items.append(the_plot[0])
            legend_ids.append(stream_labels[self.data_labels[stream_dim][ind]])

        self.ax.set_xlim(left=0)
        self.ax.set_ylabel(self.data_units[3])
        self.ax.set_xlabel(self.data_axes[time_dim])
        #self.ax.set_xticks(indList, t)
        #self.ax.set_yticks(np.arange(0,max(run_sum)))
        self.legend = self.ax.legend(legend_items,legend_ids, 
                title=self.data_axes[stream_dim], loc='upper right', 
                bbox_to_anchor=(1.25,1)) 

        return self

###############################################################################
    def river_plot(self, streamDim=None, streamList=None,
                   selectDim=None, selectItem=None):
        """
        Creates a river plot of the data in this Query. 'time' and at least one
        (but no more than two) other axes must exist.

        Plots the data in the dimension 'streamDim' against time. If a
        streamList of label names of items in the streamDim axis is given, we
        plot
        only those streams instead of all of them. If the data array is
        currently
        three dimensional, the user must also specify a dimension, selectDim,
        and a label of an element in that dimension, selectItem, to plot.

        3D example:

        To make an isotope-wise river plot for facility 5 when the array
        looks like this:

        data_axes = ['time', 'thru', 'iso']
        data_units = ['months', 'facID', 'tons']

        call

        q.river_plot(streamDim = 'iso', selectDim = 'thru', selectItem = 5)
        """

        time_dim, stream_dim, select_dim, stream_list = \
                self.check_plottable(streamDim, streamList, selectDim)

        plot_data = self.prep_data(stream_dim, select_dim, selectItem)

        self.set_up_figure()

        # get time dimension labels
        t = self.data_labels[time_dim]  

        # colors = get_colours(len(stream_list))
        # For RANDOM colors:
        colors = pylab.rand(len(stream_list),len(stream_list))

        # Turn the list of stream labels into a list of indices.
        indList = [0] * len(stream_list)
        for i, s in enumerate(stream_list):
            indList[i] = self.data_labels[stream_dim].index(s)

        # Iterate through the streams and add them to the plot.
        for ind in indList:
            self.ax.fill_between(t, runSum, runSum + plot_data[:, ind],
                                 facecolor=colors[ind], alpha=0.9,
                                 label=str(ind))
            runSum += plot_data[:, ind]

        # Override the default x-axis behavior.
        self.ax.set_xlim(xmin=self.t0, xmax=self.tf)

        # Use a reasonable scale on the y axis
        graphLim = max(runSum) * 1.05
        # graphLim = mean(runSum)
        self.ax.set_ylim(ymin=0.01, ymax=graphLim)
        # ax.set_ylim(ymax=)
        self.ax.set_title(self.data_axes[select_dim] + " = " + str(selectItem))

        return self

###############################################################################

    def save_plot(self, filename=''):
        """
        Saves the figure currently stored in this Query object, or throws a
        QueryException if none exists. The argument is the desired filename.
        """

        if None == self.figure:
            raise QueryException("Error: this Query hasn't been asked to " +
                                 "plot anything.")

        if '' == filename:
            raise QueryException(
                "Error: please give filename for plot output.")


        fig = self.figure

        if self.legend is not None : 
            pylab.savefig(filename, bbox_extra_artists=(self.legend,), 
                    bbox_inches='tight')
        else :
            pylab.savefig(filename)


        return self

###############################################################################
    def clear_fig(self):
        self.ax = None
        self.figure = None
        pylab.close()

###############################################################################
###############################################################################


class sql_stmt(object):
    """
    A class for storing the text of SQL statements to be executed.
    """

    sStr = ''
    fStr = ''
    wStr = ''
    oStr = ''

###############################################################################

    def __init__(self, s, f, w=None, o=None):
        """
        Initializes the SELECT, FROM, and (optionally) WHERE clauses of the
        sql_stmt.  Do not include the keywords themselves in the clauses, but
        do
        separate items by commas and use standard syntax.
        """

        self.sStr = 'SELECT ' + s + ' '
        self.fStr = 'FROM ' + f + ' '
        if None != w:
            self.wStr = 'WHERE ' + w
        if None != o:
            self.oStr = 'ORDER BY' + o

###############################################################################

    def __str__(self):
        return self.sStr + self.fStr + self.wStr + self.oStr

###############################################################################

    def replace_select(self, columns):

        self.sStr = 'SELECT' + columns + ' '

###############################################################################

    def replace_from(self, tables):

        self.fStr = 'FROM' + tables + ' '

###############################################################################

    def replace_where(self, conditions):

        self.wStr = 'WHERE' + conditions + ' '

###############################################################################

    def replace_order(self, sorting):

        self.oStr = 'ORDER BY ' + sorting + ' '

###############################################################################
###############################################################################


class QueryException(Exception):
    """
    An exception class for use by the Query class in the GENIUS GenPost module
    """

    def __init__(self, value):
        """Creates a new QueryException"""
        self.value = value

    def __str__(self):
        """Returns a string representation of this QueryException"""
        return repr(self.value)

###############################################################################
###############################################################################


def get_iso_list():
    return [
        8016,
        2004,
        88226,
        88228,
        82206,
        82207,
        82208,
        82210,
        90228,
        90229,
        90230,
        90232,
        83209,
        89227,
        91231,
        92232,
        92233,
        92234,
        92235,
        92236,
        92238,
        93237,
        94238,
        94239,
        94240,
        94241,
        94242,
        94244,
        95241,
        952421,
        95243,
        96242,
        96243,
        96244,
        96245,
        96246,
        96247,
        96248,
        96250,
        98249,
        98250,
        98251,
        98252,
        1003,
        6014,
        60129,
        36081,
        36085,
        360849,
        38090,
        380889,
        43099,
        430989,
        43097,
        53129,
        531279,
        55134,
        55135,
        55137,
        551339,
        9992409,
        8881159
    ]

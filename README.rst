###################################################
CYDER
###################################################

This repository holds a generic nuclear repository model intended to be used 
within the Cyclus nuclear fuel cycle simulator. This repository model is not meant for site specific 
nuclear repository performance assesment. Rather, it is intended to provide 
medium-fidelity performance metrics of disposal system concepts in generic 
geologies within the broader cyclus simulation framework.

This README provides basic information about:
 - the dependencies required by Cyder
 - installation of Cyder from the command line
 - how to run Cyder unit tests


.. contents:: Table of Contents


************
Dependencies
************

Cycamore's only dependency is the Cyclus Core.

====================   ==================
Package                Minimum Version
====================   ==================
`Cyclus`               1.5.3
====================   ==================

There are a number of ways to install the Cyclus core:

- To install from source code, see the `Cyclus Core repository
  <http://github.com/cyclus/cyclus>`_

- To install from a binary distribution, see the instructions for
  `Installing Cyclus from Binaries <DEPENDENCIES.rst>`_

- **For general information about Cyclus, visit the**  `Cyclus Homepage`_,

- **For detailed installation instructions, visit the**
  `INSTALLATION Guide <INSTALL.rst>`_,

- **To see user and developer documentation for this code, please visit
  the** `Users Guide <http://fuelcycle.org/user/index.html>`_,

******************************
Running Tests
******************************

Installing Cyder will also install a test driver (i.e., an executable of all of
our tests). You can run the tests yourself via:

.. code-block:: bash

    $ cyder_unit_tests


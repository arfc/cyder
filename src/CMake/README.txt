_______________________________________________________________________
Cyder
_______________________________________________________________________

This repository holds a generic nuclear repository model intended to be used 
within the Cyclus nuclear fuel cycle simulator from the University of 
Wisconsin - Madison. This repository model is not meant for site specific 
nuclear repository performance assesment. Rather, it is intended to provide 
medium-fidelity performance metrics of disposal system concepts in generic 
geologies within the broader cyclus simulation framework.

To see user and developer documentation for this code, please visit the `Cyder Homepage`_.


-----------------------------------------------------------------------
LISCENSE
-----------------------------------------------------------------------

::

    Copyright (c) 2010-2012, University of Wisconsin Computational Nuclear Engineering Research Group
     All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    
      - Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.
      
      - Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      
      - Neither the name of the University of Wisconsin Computational
        Nuclear Engineering Research Group nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

------------------------------------------------------------------
Building Cyder
------------------------------------------------------------------

The `Cyder Homepage`_ has much more detailed guides and information.
This Readme is intended to be a quick reference for building cyclus for the
first time.

The Cyder code requires the following software and libraries.

====================   ==================
Package                Minimum Version   
====================   ==================
`Cyclus`               0.1 
`CMake`                2.8            
`boost`                1.34.1
`libxml2`              2                 
`sqlite3`              3.7.10            
====================   ==================

~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Building and Running Cyder
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to facilitate future compatibility with multiple platforms, Cyder is
built using  `Cmake <http://www.cmake.org>`_. This relies on CMake version
2.8 or higher and the CMakeLists.txt file in `src/`. It is
recommended that you use CMake to build the Cyder external to the
source code. To do this, execute the following steps::

    .../cyder/$ mkdir build
    .../cyder/$ cd build
    .../cyder/build$ cmake ../src 

You should see output like this::

    ...
    ...
    >> -- Configuring done
    >> -- Generating done
    >> -- Build files have been written to: .../cyder/build
    /core/build$ make cyder
    >> Scanning dependencies of target cyder
    ...
    ...
    >> [100%] Building CXX object CMakeFiles/cyder.dir/GenericRepository.cpp.o
    >> Linking CXX executable cyder
    >> [100%] Built target cyder

Now, you need to make and install the library in the cyclus module path. To do 
so, execute the following steps ::
    
    .../cyder/build$ make 
    .../cyder/build$ make install

 If the installation directory is a system path, the ```make install``` step may 
 need to be ```sudo make install```.

This module is a Facility type module, so it is installed in the cyclus/Models/Facility 
directory where cyclus is installed.

Now, you can run your installed cyclus program with an input file that defines a Generic Repository.

The `Cyclus Homepage`_ has much more detailed guides and information.  If
you intend to develop for *Cyclus*, please visit it to learn more.


.. _`Cyder Homepage`: http://cyder.github.com
.. _`Cyclus Homepage`: http://cyclus.github.com




Building irrODE Wrapper for MacOSX - 2010-6-25
==============================================

This document is meant as a HowTo guide to build the irrODE Wrapper for MacOSX.

0.) Requirements
I used XCode 3.2, Irrlicht 1.7.1, ODE 0.11.1 and the current irrODE wrapper. 
You must build libIrrlicht.a and libODE.a in advance for your desired configuration 
in seperate XCode projects! I tried to use the ODE library from MacPorts and got 
runtime errors, although the linking worked. In my case, the configuration was 
set to "i386 debug" for the current OSX SDK (1.6).


1.) Building ODE
1.1) download ODE from http://sourceforge.net/projects/opende and unzip -> ode-0.11.1
1.2) run ./configure to create config.h etc.
1.3) create a XCode empty project in ode-0.11.1, I called it MacOSX
1.4) add a new target (Cocoa static library) named ODE, which will compile to libODE.a
1.5) add the following directories to the project:
+ ode (remove all other directories/files other than src to only build the library)
+ OPCODE
+ include
1.6) uncheck or remove Makefile files in ode and OPCODE directories to avoid build problems
1.7) edit project settings (for All Configurations):
* remove the prefix header "GCC_PREFIX_HEADER" or "Prefix Header".
  Here is an AppKit.framework prefix header inserted by default that will lead to compilation issues!
* uncheck the "Precompile Prefix Header" option
* you can remove "Other Linker Flags"
* Set "Other C++ Flags": -DdNODEBUG -DdTRIMESH_OPCODE -DdTRIMESH_ENABLED
1.8) build the project and there you are hopefully!


2.) Build Irrlicht using the provided MacOSX project, download from:
http://irrlicht.sourceforge.net/


3.) download latest irrODE library with following SVN checkout: 
svn co https://irrode.svn.sourceforge.net/svnroot/irrode irrode

3.1) building the XCode project should function if you have the following directory structure:
+ MyIrrlicht3dProject
+- irrlicht-1.7.1
+- irrode
+- ode-0.11.1

3.2) Open the XCode project named "XCode", to build the library (libirrODE.a)
and the executables (irrOdeHello, irrOdePlayground and irrOdeCar)
Maybe your directory structure differs from the one shown above, then you must 
set the correct "search header paths" and library paths in the XCode project.

3.3) Adjustments
I had to apply some small changes to the source of the irrODE wrapper to be able to 
use it on OSX. 

a) The only working renderer is EDT_OPENGL, so you should set this in the 
executables to open the window.

b) you must copy the data folder into the MacOSX folder in order to have the examples 
find the resource files. Or you can make a symbolic link by terminal:
go to the MacOSX directory and type "ln -s ../data/ data" to achieve that.

3.4) Possible Problems
I had following issues in my tests:

a) In the IrrOdePlayground, using Trimesh-Terrain, there were no collisions. 
Using the other option (Heightfield?) works.

Solution: In ODE, the compile option for TRIMESH and OPCODE must be set!

b) In the irrOdeCar Demo, I had to disable the Plane (in the /data/irrOdeCar.xml file)
and in the irrodecar_main.cpp. On loading, there was an error (Assertion failed) in the ODE function 
dNormalize, but I couldn't define, which was the offending vector.

Solution; in ODE, the compile option NODEBUG must be set, so that Assertions do not fail.


4.) Acknowledgements
Thanks to the Irrlicht team for this great engine and Brainsaw for the irrOde 
Wrapper, which is the best documented physics wrapper I could find. 



Building irrODE Wrapper for MacOSX - 2010-6-10
==============================================

This document is meant as a HowTo guide to build the irrODE Wrapper for MacOSX.
As a warning, I am new to XCode, OSX development and Irrlicht, but I still got it 
to work (with limitations, see below), so it must be easy :)

1.) Requirements
I used XCode 3.2, Irrlicht 1.7.1, ODE 0.11.1 and the current irrODE wrapper. 
You must build libIrrlicht.a and libODE.a in advance for your desired configuration 
in seperate XCode projects! I tried to use the ODE library from MacPorts and got 
runtime errors, although the linking worked. In my case, the configuration was 
set to "i386 debug" for the current OSX SDK (1.6).

Here is the directory structure I used:
+ irrlicht-1.7.1
+ irrODE
+ ode-0.11.1

2.) Open the project
Now you can open the XCode project named "MacOSX", to build the library (libirrODE.a)
and the executables (irrOdeHello, irrOdePlayground and irrOdeCar)
Maybe your directory structure differs from the one shown above, then you must 
set the correct "search header paths" and library paths in the XCode project.

3.) Adjustments
I had to apply some small changes to the source of the irrODE wrapper to be able to 
use it on OSX. 

a) The only working renderer is EDT_OPENGL, so you should set this in the 
executables to open the window.

b) The irrODE uses getch() in "conio.h" which is not defined in MacOSX. You can 
change the statement "c=getch();" to "std::cin >> c;" and replace "conio.h" by "<iostream>".
Or you can simply set c to the desired value, f.i. "c='y';".

c) you must copy the data folder into the MacOSX folder in order to have the examples 
find the resource files. Or you can make a symbolic link by terminal:
go to the MacOSX directory and type "ln -s ../data/ data" to achieve that.

4.) Limitations
I had following issues in my tests:

a) In the IrrOdePlayground, using Trimesh-Terrain, there were no collisions. 
Using the other option (Heightfield?) works.

b) In the irrOdeCar Demo, I had to disable the Plane (in the /data/irrOdeCar.xml file)
and in the irrodecar_main.cpp. On loading, there was an error (Assertion failed) in the ODE function 
dNormalize, but I couldn't define, which was the offending vector.

Also, the collision didn't always work, just one of the ramps in the simple car example
would collide. 

5.) Acknowledgements
Thanks to the Irrlicht team for this great engine and Brainsaw for the irrOde 
Wrapper, which is the best documented physics wrapper I could find. 

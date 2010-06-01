http://spacegroup.wordpress.com/

Hi all,
I did this library to compile irrOde under Visual Studio 2005.
You MUST replace the default libIrrOde.lib with that, and finally...you can compile irrOde with Visual Studio 2005!
TIP: You can also ignore to compile Ode Engine, the lib ode_single.lib is in "lib" directory

If you want to re-compile the lib under visual studio, you must open the .sln file. 
IMPORTANT NOTE: 
YOU NEED TO SET THAT INCLUDE FOLDERS: 
- Irrlicht 1.7.1 "include" folder
- Ode 0.11.1 "include" folder
- IrrOde "include" folder

Without that includes, the lib won't compile.
Email me-> stefano@rpgmaking.net

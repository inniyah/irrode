mkdir irrOde
mkdir irrode\bin
mkdir irrode\bin\gcc-win32
copy bin\gcc-win32\*.exe irrode\bin\gcc-win32
copy bin\gcc-win32\*.dll irrode\bin\gcc-win32
copy bin\gcc-win32\irrklang.dll irrode\bin\gcc-win32
copy bin\gcc-win32\libirrode.a irrode\bin\gcc-win32
copy bin\gcc-win32\libode_single.a irrode\bin\gcc-win32
del irrode\bin\gcc-win32\*_debug.exe
del irrode\bin\gcc-win32\libIrrOdeNull.a
mkdir irrode\bin\gcc-win32\RePlayerPlugins
copy bin\gcc-win32\RePlayerPlugins\*.* irrode\bin\gcc-win32\RePlayerPlugins
mkdir irrode\bin\vc2008
copy bin\vc2008\*.dll irrode\bin\vc2008
copy bin\vc2008\libIrrOde.lib irrode\bin\vc2008
copy bin\vc2008\ode_single.lib irrode\bin\vc2008
copy bin\vc2008\*.exe irrode\bin\vc2008
mkdir irrode\bin\vc2005
copy bin\vc2005\*.* irrode\bin\vc2005
copy *.txt irrode
copy changelog.txt irrode
mkdir irrode\data
copy data\*.* irrode\data
del irrode\data\irrOdeCarSettings.xml
del irrode\data\irrOdeCarControls.xml
del irrode\data\highscore.xml
copy irrode\data\_highscore.xml irrode\data\highscore.xml
copy irrode\data\_irrOdeCarSettings.xml irrode\data\irrOdeCarSettings.xml
del irrode\data\_highscore.xml
mkdir irrode\data\irrodedit
copy data\irrodedit\*.* irrode\data\irrodedit
mkdir irrode\data\replay
copy data\replay\*.txt irrode\data\replay
mkdir irrode\data\instruments
copy data\instruments\*.* irrode\data\instruments
del irrode\data\instruments\*.xcf
mkdir irrode\data\warnlights
copy data\warnlights\*.* irrode\data\warnlights
del irrode\data\warnlights\*.xcf
mkdir irrode\data\heli
copy data\heli\*.* irrode\data\heli
mkdir irrode\data\car
copy data\car\*.* irrode\data\car
del irrode\data\car\*.xcf
mkdir irrode\data\sound
copy data\sound\*.ogg irrode\data\sound
mkdir irrode\data\models
copy data\models\*.* irrode\data\models
mkdir irrode\data\roads
copy data\roads\*.* irrode\data\roads
mkdir irrode\data\textures
copy data\textures\*.* irrode\data\textures
mkdir irrode\data\textures\buttons
copy data\textures\buttons\*.png irrode\data\textures\buttons
mkdir irrode\data\shaders
copy data\shaders\*.* irrode\data\shaders
mkdir irrode\data\scenes
copy data\scenes\*.* irrode\data\scenes
mkdir irrode\data\parameters
copy data\parameters\*.* irrode\data\parameters

mkdir irrode\source
mkdir irrode\source\IrrOde
copy source\IrrOde\*.* irrode\source\IrrOde
mkdir irrode\source\IrrOde\event
copy source\IrrOde\event\*.* irrode\source\IrrOde\event
mkdir irrode\source\IrrOde\geom
copy source\IrrOde\geom\*.* irrode\source\IrrOde\geom
mkdir irrode\source\IrrOde\joints
copy source\IrrOde\joints\*.* irrode\source\IrrOde\joints
mkdir irrode\source\IrrOde\motors
copy source\IrrOde\motors\*.* irrode\source\IrrOde\motors
mkdir irrode\source\IrrOde\observer
copy source\IrrOde\observer\*.* irrode\source\IrrOde\observer
mkdir irrode\source\IrrOdeCar
copy source\IrrOdeCar\*.* irrode\source\IrrOdeCar
mkdir irrode\source\IrrOdeRePlayer
copy source\IrrOdeRePlayer\*.* irrode\source\IrrOdeRePlayer

mkdir irrode\include
mkdir irrode\include\IrrOde
copy include\IrrOde\*.* irrode\include\IrrOde
mkdir irrode\include\IrrOde\event
copy include\IrrOde\event\*.* irrode\include\IrrOde\event
mkdir irrode\include\IrrOde\geom
copy include\IrrOde\geom\*.* irrode\include\IrrOde\geom
mkdir irrode\include\IrrOde\joints
copy include\IrrOde\joints\*.* irrode\include\IrrOde\joints
mkdir irrode\include\IrrOde\motors
copy include\IrrOde\motors\*.* irrode\include\IrrOde\motors
mkdir irrode\include\IrrOde\observer
copy include\IrrOde\observer\*.* irrode\include\IrrOde\observer

mkdir irrode\include\IrrOdeCar
copy include\IrrOdeCar\*.* irrode\include\IrrOdeCar
mkdir irrode\include\IrrOdeRePlayer
copy include\IrrOdeRePlayer\*.* irrode\include\IrrOdeRePlayer
mkdir irrode\include\IrrOdeCar\RoadCreator
copy include\IrrOdeCar\RoadCreator irrode\include\IrrOdeCar\RoadCreator

mkdir irrode\codeblocks
copy codeblocks\*.* irrode\codeblocks
mkdir irrode\vc2008
copy vc2008\*.* irrode\vc2008
del irrode\vc2008\*.ncb
del irrode\vc2008\*.user
mkdir irrode\vc2005
copy vc2005\*.* irrode\vc2005
mkdir irrode\XCode
copy XCode\*.* irrode\XCode
mkdir irrode\XCode\MacOSX.xcodeproj
copy XCode\MacOSX.xcodeproj\*.* irrode\XCode\MacOSX.xcodeproj
mkdir irrode\XCode\res 
mkdir irrode\XCode\res\MainMenu.nib
copy XCode\res\MainMenu.nib\*.* irrode\XCode\res\MainMenu.nib

mkdir irrode\tutorials
mkdir D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials
copy tutorials\*.* irrode\tutorials
copy tutorials\*.* D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials

mkdir irrode\tutorials\01-ProjectSetup-gcc
mkdir D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\01-ProjectSetup-gcc
copy tutorials\01-ProjectSetup-gcc\*.* irrode\tutorials\01-ProjectSetup-gcc
copy tutorials\01-ProjectSetup-gcc\*.* D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\01-ProjectSetup-gcc

mkdir irrode\tutorials\01-ProjectSetup-gcc\pix
mkdir D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\01-ProjectSetup-gcc\pix
copy tutorials\01-ProjectSetup-gcc\pix\*.* irrode\tutorials\01-ProjectSetup-gcc\pix
copy tutorials\01-ProjectSetup-gcc\pix\*.* D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\01-ProjectSetup-gcc\pix

mkdir irrode\tutorials\02-ProjectSetup-vc
mkdir D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\02-ProjectSetup-vc
copy tutorials\02-ProjectSetup-vc\*.* irrode\tutorials\02-ProjectSetup-vc
copy tutorials\02-ProjectSetup-vc\*.* D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\02-ProjectSetup-vc

mkdir irrode\tutorials\02-ProjectSetup-vc\pix
mkdir D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\02-ProjectSetup-vc\pix
copy tutorials\02-ProjectSetup-vc\pix\*.* irrode\tutorials\02-ProjectSetup-vc\pix
copy tutorials\02-ProjectSetup-vc\pix\*.* D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\02-ProjectSetup-vc\pix

mkdir irrode\tutorials\03-irrOdEdit
mkdir D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\03-irrOdEdit
copy tutorials\03-irrOdEdit\*.* irrode\tutorials\03-irrOdEdit
copy tutorials\03-irrOdEdit\*.* D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\03-irrOdEdit

mkdir irrode\tutorials\03-irrOdEdit\pix
mkdir D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\03-irrOdEdit\pix
copy tutorials\03-irrOdEdit\pix\*.* irrode\tutorials\03-irrOdEdit\pix
copy tutorials\03-irrOdEdit\pix\*.* D:\xampp\htdocs\homepage\bulletbyte\irrOde\tutorials\03-irrOdEdit\pix

mkdir irrode\doxygen
copy doxygen\*.* irrode\doxygen
mkdir irrode\doxygen\html
copy ..\_doxygen\IrrOde\html\*.* irrode\doxygen\html

C:\Programme\7-Zip\7z.exe a -r -tzip irrode.zip irrode\*
pause
rmdir irrode /S /Q
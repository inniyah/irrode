rmdir ..\_doxygen\IrrOde /S /Q
mkdir ..\_doxygen\IrrOde
copy doxygen\IrrOde.doxy ..\_doxygen\IrrOde
C:\Programme\doxygen\bin\doxygen.exe ..\_doxygen\IrrOde\IrrOde.doxy
del D:\xampp\htdocs\homepage\bulletbyte\irrOde\doxygen\*.* /Q
xcopy ..\_doxygen\IrrOde\html D:\xampp\htdocs\homepage\bulletbyte\irrOde\doxygen /S /E /Y
mkdir irrode\doxygen
xcopy ..\_doxygen\IrrOde\*.* irrode\doxygen /E

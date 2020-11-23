echo off
cls
rem Ugly ugly script to download sdl2 and setup project on Windows, wheee
rem Ensure SDL2 is setup, if not download it and awkwardly put it together
if not exist .\src\3rdparty\sdl2\ (
	rem Download and extract SDL2 data
	echo SDL2 dir not found under 3rdparty, downloading...
	powershell -Command "Invoke-WebRequest https://www.libsdl.org/release/SDL2-devel-2.0.9-mingw.tar.gz -OutFile .\src\3rdparty\SDL2-devel-2.0.9-mingw.tar.gz"
	echo Extracting data...
	powershell -Command "tar -x -v -C .\src\3rdparty\ -f .\src\3rdparty\SDL2-devel-2.0.9-mingw.tar.gz"
	echo Deleting downloaded package...
	del .\src\3rdparty\SDL2-devel-2.0.9-mingw.tar.gz
	echo Done!

	rem SDL2 has been downloaded, now we have to shuffle everything
	echo Creating sdl2 directory and copying data...
	mkdir .\src\3rdparty\sdl2\
	mkdir .\src\3rdparty\sdl2\include\
	mkdir .\src\3rdparty\sdl2\lib\
	Xcopy /E /I .\src\3rdparty\SDL2-2.0.9\x86_64-w64-mingw32\include .\src\3rdparty\sdl2\include
	Xcopy /E /I .\src\3rdparty\SDL2-2.0.9\x86_64-w64-mingw32\lib .\src\3rdparty\sdl2\lib
	echo Done!

	rem Now delete the SDL2-2.0.9 dir
	del /F /Q /S .\src\3rdparty\SDL2-2.0.9\*.*
	rmdir /S /Q .\src\3rdparty\SDL2-2.0.9\
)
pause

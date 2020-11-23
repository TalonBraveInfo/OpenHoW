echo off
cls
if not exist .\build\ mkdir .\build\
cd .\build\
cmake .\..\
pause

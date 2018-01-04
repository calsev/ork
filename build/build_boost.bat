REM  Run in $(BOOST_ROOT)
REM  This script builds dynamic libs for recent versions of VS (give it some time!)

if not exist .\bin mkdir .\bin
if not exist .\lib mkdir .\lib  

REM Visual Studio 2017
b2 toolset=msvc-14.1 address-model=64 link=shared runtime-link=shared --build-type=complete --without-mpi stage

REM Visual Studio 2015
b2 toolset=msvc-14.0 address-model=64 link=shared runtime-link=shared --build-type=complete --without-mpi stage

move /Y .\stage\lib\*.dll .\bin
move /Y .\stage\lib\*.lib .\lib

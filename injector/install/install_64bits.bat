@echo off
set	"search=bincrafters:"
for /f %%i in ('conan remote list') do set VAR=%%i
if "%VAR%"=="%search%" goto :found
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
echo "added source: bincrafters"
:found

mkdir ..\build && cd ..\build && conan install --build=missing -s compiler="Visual Studio" -s compiler.version=15 -s "arch=x86_64" -s "arch_build=x86_64" .. && cmake .. -G "Visual Studio 16 2019" -A x64 -Thost=x64 -DCMAKE_BUILD_TYPE=Release
echo "Visual Studio Solution File created unto \build"
pause

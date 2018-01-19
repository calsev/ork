
:: These variables must match the appveyor environment
:: We are only running one configuration for now
cd ../..
set REPO_ROOT=%cd:\=/%
cd %~dp0
set ORK_UNICODE=ON
set VS_VERSION=15 2017
set VS_TOOLSET=141

if not exist "%REPO_ROOT%/ork_test_141" mkdir "%REPO_ROOT%/ork_test_141"
if not exist "%REPO_ROOT%/ork_test_141/bin" mkdir "%REPO_ROOT%/ork_test_141/bin"
if not exist "%REPO_ROOT%/ork_test_141/build" mkdir "%REPO_ROOT%/ork_test_141/build"

cd "%REPO_ROOT%/ork_test_141/build"
cmake -G "Visual Studio 15 2017 Win64" -T v141 "%REPO_ROOT%/ork/test"

:: Return from whence we came, ready to run again
cd %~dp0

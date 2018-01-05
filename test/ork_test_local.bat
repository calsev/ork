:: This script runs the ork test suite locally.
:: Because ork builds and uses sibling dependencies, this requires some environment setup.
:: The environment created here must be reproduced in appveyor for automated tests to work.
:: This script assumes a local repo setup exists that is functionally equivalent to that below.
::
:: repo_root
:: |-boost_141 (git repo)
:: |-catch (git repo)
:: |-glm (git repo)
:: |-json (git repo)
:: |-ork (git repo)
:: |-ork_test_141 (created here)
::   |-bin (created here)
::   |-build (created here)
:: |-pugi (git repo)
:: |-yaml (git repo)

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

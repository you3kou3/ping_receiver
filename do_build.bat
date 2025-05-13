

@rem set BUILD_TYPE=Debug
@rem set BUILD_TYPE=RelWithDebInfo
@rem set BUILD_TYPE=Release

@echo on

set BUILD_TYPE=RelWithDebInfo
set INSTALL_TYPE=rel

set SOURCE=%~dp0
set BUILD=%~dp0build
set VCPKG=%~dp0\vcpkg
set BINARY=%~dp0bin

PATH %VCPKG%;%PATH%
set CMAKE_OPTS= ^
  -DCMAKE_TOOLCHAIN_FILE=%VCPKG%/scripts/buildsystems/vcpkg.cmake ^
  -DVCPKG_TARGET_TRIPLET=x64-windows ^
  -DINSTALL_TYPE=%INSTALL_TYPE% ^
  %CMAKE_OPTS%

if not exist "%BUILD%" mkdir "%BUILD%"

pushd "%BUILD%"
cmake ^
  -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
  %CMAKE_OPTS% ^
  %SOURCE%
@if ERRORLEVEL 1 goto :END

cmake --build . --config %BUILD_TYPE%
@if ERRORLEVEL 1 goto :END



@popd
if not exist "%BINARY%" mkdir "%BINARY%"

del /Q "%BINARY%\*"
copy "%BUILD%\%BUILD_TYPE%\*" "%BINARY%\"
echo ping_receiver.exe 8.8.8.8 > "%BINARY%\run.bat"

:END
@popd


@rem boostを使いたいので
@rem vcpkgをcloneしてビルドする
@rem 実行時間かかります

set VCPKG_DEFAULT_BINARY_CACHE=%~dp0build_vcpkg
if not exist "%VCPKG_DEFAULT_BINARY_CACHE%" mkdir "%VCPKG_DEFAULT_BINARY_CACHE%"

if not exist "vcpkg" git clone https://github.com/microsoft/vcpkg


@pushd %~dp0vcpkg

call .\bootstrap-vcpkg.bat
.\vcpkg install --triplet=x64-windows ^
  boost 

@popd

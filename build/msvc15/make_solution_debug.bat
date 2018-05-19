@echo off

set "CONFIGURATION=Debug"
set "SOLUTION_DIR=..\solutions\libfpf_msvc15_%CONFIGURATION%"

@pushd ..
@mkdir %SOLUTION_DIR%
@pushd %SOLUTION_DIR%
CMake -G "Visual Studio 15 2017" "%CD%\..\.." -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% -DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION% -DLIBFPF_EXAMPLES_BUILD:BOOL=TRUE
@popd
@popd

@echo on
@pause
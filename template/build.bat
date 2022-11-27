@echo off

set ProjectName=PigWasmTemplate

set DebugBuild=1
set DeveloperBuild=1
set CopyToDataDirectory=1
set AssertionsEnabled=1

set SourceDirectory=..\source
set DataDirectory=..\data
set BuildDirectory=..\build
set LibDirectory=..\lib
set JavascriptFolder=..\lib\include\pig_wasm\js
set ViewFolder=..\lib\include\pig_wasm\view

set CompilerFlags=-DWASM_COMPILATION -DDEBUG_BUILD=%DebugBuild% -DDEVELOPER_BUILD=%DeveloperBuild% -DDEMO_BUILD=0 -DSTEAM_BUILD=0 -DEARLY_ACCESS_BUILD=0 -DASSERTIONS_ENABLED=%AssertionsEnabled% -DSLUG_SUPPORTED=0 -DBOX2D_SUPPORTED=0 -DPROCMON_SUPPORTED=0 -DOPENGL_SUPPORTED=0 -DVULKAN_SUPPORTED=0 -DDIRECTX_SUPPORTED=0
rem -Wenum-compare-switch = comparison of different enumeration types in switch statement
set CompilerFlags=%CompilerFlags% -Wno-enum-compare-switch
rem --target=wasm32       = Target the WASM32 instruction set?
rem -O0                   = No optimizations?
rem -c                    = Compile only, no link?
rem --no-standard-include = ?
set CompilerFlags=%CompilerFlags% --target=wasm32 -mbulk-memory -O0 --no-standard-includes
set IncludeDirectories=-I"%SourceDirectory%" -I"%LibDirectory%\include" -I"%LibDirectory%\include\pig_wasm" -I"%LibDirectory%\include\pig_wasm\std"
rem --no-entry        = ?
rem --export-all      = ?
rem --allow-undefined = ?
rem --no-gc-sections  = ?
rem --import-memory   = ?
rem --lto-O2          = ?
set LinkerFlags=--no-entry --allow-undefined --import-memory --lto-O2

echo [Running on %ComputerName%]

echo Compiling %ProjectName%.o...
clang %SourceDirectory%\compile_me.cpp -c %CompilerFlags% %IncludeDirectories% -o %ProjectName%.o
echo Linking...
wasm-ld %ProjectName%.o %LinkerFlags% -o %ProjectName%.wasm
python "CombineFiles.py" "%BuildDirectory%\glue_combined.js" "%JavascriptFolder%\glue_defines.js" "%JavascriptFolder%\glue_helpers.js" "%JavascriptFolder%\glue_api.js" "%JavascriptFolder%\glue.js"

if "%CopyToDataDirectory%"=="1" (
	echo [Copying %ProjectName%.wasm to data directory]
	XCOPY ".\%ProjectName%.wasm" "%DataDirectory%\" /Y > NUL
	XCOPY "%ViewFolder%\index.html" "%DataDirectory%\" /Y > NUL
	XCOPY "%ViewFolder%\main.css" "%DataDirectory%\" /Y > NUL
	XCOPY "%BuildDirectory%\glue_combined.js" "%DataDirectory%\" /Y > NUL
	rem TODO: Can we mark the glue_combined.js file as javascript MIME type?
)

@echo off
rem Build and install the TMGDL library.
rem usage: BUILD [TMT|BPC|FPC]   (default: TMT, see MACROS)
set MKOPT=
if "%1"=="BPC" set MKOPT=-DUSE_BPC
if "%1"=="bpc" set MKOPT=-DUSE_BPC
if "%1"=="FPC" set MKOPT=-DUSE_FPC
if "%1"=="fpc" set MKOPT=-DUSE_FPC
make %MKOPT% clean
make %MKOPT%
if errorlevel 1 goto failed
make %MKOPT% install
goto end
:failed
echo Build failed, the library was not installed.
:end
set MKOPT=

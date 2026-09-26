@echo off
rem Build the '2048' game.
rem usage: BUILD [TMT|BPC|FPC]   (default: TMT, see ..\..\SRC\MACROS)
set MKOPT=
if "%1"=="BPC" set MKOPT=-DUSE_BPC
if "%1"=="bpc" set MKOPT=-DUSE_BPC
if "%1"=="FPC" set MKOPT=-DUSE_FPC
if "%1"=="fpc" set MKOPT=-DUSE_FPC
make %MKOPT% clean
make %MKOPT%
if errorlevel 1 goto failed
echo Done. Run 'make %MKOPT% install' to copy the files to the BIN folder.
goto end
:failed
echo Build failed.
:end
set MKOPT=

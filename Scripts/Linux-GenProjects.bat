@echo off
pushd %~dp0\..\
call Bolt\vendor\bin\premake\premake5.exe --os=linux gmake2
popd
PAUSE
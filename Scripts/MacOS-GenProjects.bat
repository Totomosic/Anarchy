@echo off
pushd %~dp0\..\
call Bolt\vendor\bin\Windows\premake\premake5.exe --os=macosx xcode4
popd
PAUSE
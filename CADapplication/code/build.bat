@echo off

pushd ..\build

cl -nologo -Zi -FC -WX -DDEV ../code/CADapplication.cpp/link user32.lib gdi32.lib -opt:ref

popd

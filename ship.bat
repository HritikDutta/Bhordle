@echo off

set exportDir= export

echo Building...
call build release
echo Build Finished successfully!
echo.

if not exist %exportDir% md %exportDir%
if not exist %exportDir%\assets md %exportDir%\assets

echo Copying executable...
xcopy /y wordle.exe %exportDir%
echo.

echo Copying assets...
xcopy /y /s assets %exportDir%\assets
echo.
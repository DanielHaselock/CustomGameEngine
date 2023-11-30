@echo off

@REM Create the build directory if it doesn't exist
@echo Looking for build directory...
if not exist "build" mkdir build && echo "Created build directory"

@REM Configure the project
@REM -S: Source directory
@REM -B: Build directory
@echo Configuring project...
cmake -S "Sources\Engine" -B "build"
@echo.
@echo.
@echo Project configured!

@REM If Visual Studio not running, open the solution.
@REM Otherwise, do nothing.
@echo Opening solution...
tasklist /fi "imagename eq devenv.exe" | find /i "devenv.exe" > nul && echo Visual Studio is already running || start build/Engine.sln

@REM path: GenerateProjects.bat
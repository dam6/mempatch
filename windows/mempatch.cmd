@echo off
setlocal enabledelayedexpansion

for /f "tokens=*" %%i in ('finder.exe %1 %2 %3 %4') do (
    writer.exe %1 %%i %5
)

endlocal

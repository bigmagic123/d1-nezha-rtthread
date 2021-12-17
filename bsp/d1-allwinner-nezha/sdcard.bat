@echo off

if "%1" EQU "" (
    echo Please input you sdcard volumn! sucn as "sdcard.bat G"
) else (
    echo copy rtthread.bin to volumn:%1
    copy rtthread.bin %1:
)

@echo on

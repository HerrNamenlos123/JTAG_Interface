@echo off

:: %~dp0 is the directory where this .bat file is located
set sourcePath="%~dp0FPGA\projects\JTAG_Interface\output_files\MKRVIDOR4000.ttf"
set targetPath="%~dp0Arduino\Example\fpga_bitstream.h"
set reverseBytePath="%~dp0"

if not exist %sourcePath% ( 
    echo [91mFailed to convert: Source file not found: %sourcepath%[0m 
    Pause
    exit 1
)

echo [92mRemoving old FPGA Bitstream...[0m
if exist %targetPath% ( del %targetPath% )

echo [92mConverting new FPGA Bitstream...[0m
echo.
cmd /c "cd %reverseBytePath% && java ReverseByte %sourcePath% %targetPath%"
echo.

if not exist %targetPath% (
    echo [91mError while converting![0m
    Pause
    exit 1
)

echo [92mConversion successful![0m

timeout 3
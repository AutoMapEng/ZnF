@echo off

rem path auf sdk exe
SET EXE_PATH=E:\Prj\lrc\trunk\out\vs2013_win64\bin\release\
rem SET EXE_PATH=D:\Prj\lrc\branches\8.7\out\vs2013_win64\bin\release\

rem Pfad in dem die Testdaten liegen
SET SAMPLE_PATH=D:\Scans Softwaretest\SDK-Sync\

rem Logging
SET LOG="%SAMPLE_PATH%log.txt"

echo Report > %LOG%

SET TEST_FILES=test_pubx
FOR %%s IN (%TEST_FILES%) DO (
 echo Testfile %%s in %SAMPLE_PATH% executed with %EXE_PATH%
 SET SRC=%SAMPLE_PATH%%%s.zfs
 SET OUT=%SAMPLE_PATH%%%s.asc
 echo ----- %EXE_PATH%SampleZFS.exe -i "%SRC%" -o "%OUT%" -profiler -sync -subsample 10 ----- >> %LOG%
 %EXE_PATH%SampleZFS.exe -i "%SRC%" -o "%OUT%" -profiler -sync -subsample 10 >> %LOG%
)

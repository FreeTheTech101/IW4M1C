@echo off
IF NOT EXIST lastrev.txt (
    echo 0 > lastrev.txt
)

set REVISION=
FOR /F "tokens=2 skip=4" %%G IN ('svn info --revision HEAD ..\..') DO IF NOT DEFINED REVISION SET REVISION=%%G
FOR /F %%H IN (lastrev.txt) DO SET LASTREVISION=%%H

echo Current revision is %REVISION%
echo Last revision is %LASTREVISION%

IF %LASTREVISION% NEQ %REVISION% (
    echo // generated by svnrev.cmd > ..\..\steam_api\buildnumber.h
    echo #define BUILDNUMBER %REVISION% >> ..\..\steam_api\buildnumber.h
    echo #define BUILDNUMBER_STR "%REVISION%" >> ..\..\steam_api\buildnumber.h
    echo %REVISION% > lastrev.txt
)
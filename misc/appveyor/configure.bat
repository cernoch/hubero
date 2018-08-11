if "%CONFIGURATION%"=="Debug" (
  cmake -Hmisc\appveyor -Bbuild\misc -A%PLATFORM%
  cmake --build build\misc -- /verbosity:minimal
  cmake -H. -Bbuild\main -A%PLATFORM% -DMEMORYCHECK_COMMAND=build\misc\Debug\CoverageHelper.exe -DMEMORYCHECK_COMMAND_OPTIONS=--sep-- -DMEMORYCHECK_TYPE=Valgrind
)
if "%CONFIGURATION%"=="Release" (
  cmake -H. -Bbuild\main -A%PLATFORM%
)

@echo off
setlocal

set "sourceDir=data soundFonts shader"
set "destinationDirs=release debug EPiano"

for %%i in (%destinationDirs%) do (
    for %%j in (%sourceDir%) do (
        xcopy /E /I /Y "%%j" "%%i\%%j\"
    )
)

echo "¸´ÖÆÍê³É£¡"
pause

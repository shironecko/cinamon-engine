@echo off

echo [*] Building build tool lib...

pushd .\

if not exist .\source cd ..\
if not exist .\source cd ..\

if not "%MSVC_CONFIGURED%" == "TRUE" (
    call .\source\build_tool\configure_msvc.bat
)

cl /nologo /Od /Zi /LD /Fe.\build\ /Fd.\build\ /Fo.\build\ ^
    /I .\source /I .\source\3rdparty /I .\source\3rdparty\SDL2\include ^
    .\source\build_tool\win_build_tool_lib.c ^
    /link /incremental:no /DLL /export:get_build_commands Kernel32.lib

popd

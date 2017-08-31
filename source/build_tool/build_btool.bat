@echo off

echo [*] Building build tool...

pushd .\

if not exist .\source cd ..\
if not exist .\source cd ..\

if not "%MSVC_CONFIGURED%" == "TRUE" (
    call .\source\build_scripts\configure_msvc.bat
)

cl /nologo /Od /Zi /Fe.\build\ /Fd.\build\ /Fo.\build\ ^
	/I .\source /I .\source\3rdparty /I .\source\3rdparty\SDL2\include ^
    .\source\build_tool\win_build_tool_platform.c ^
    /link /incremental:no

call .\source\build_tool\build_btool_lib.bat

popd

@echo off

if not exist .\source cd ..\
if not exist .\source (
	echo [ERROR] Run this script from prof root or a forder one level deep!
	exit /b 1
)

if not "%MSVC_CONFIGURED%" == "TRUE" (
    call .\source\build_scripts\configure_msvc.bat
)

set CommonLinkOptions=/incremental:no .\build\SDL2.lib 
set CommonClOptions=/nologo /W4 /wd4204 /wd4100 /wd4152 /wd4201 ^
    /Od /Oi /Zi /MTd /Fe.\build\ /Fd.\build\ /Fo.\build\ /DPT_DEV_BUILD /DGL_RIGOROUS_CHECKS ^
    /I %CD%\source /I %CD%\source\3rdparty /I %CD%\source\3rdparty\SDL2\include ^
    /I %CD%\source\3rdparty\bgfx\include /I %CD%\source\3rdparty\bx\include /I %CD%\source\3rdparty\bimg\include
REM /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.10240.0\ucrt"

set SDL_MsbuildParams=/nologo /verbosity:quiet ^
	/p:PlatformToolset=%PlatformToolset%;Configuration=Debug;Platform=x64;useenv=true;OutDir=%cd%\build\ ^
	/p:AdditionalIncludePaths=$(UniversalCRT_IncludePath)

set BGFX_MsbuildParams=/nologo /verbosity:quiet ^
	/p:PlatformToolset=%PlatformToolset%;Configuration=Debug;Platform=x64;useenv=true;OutDir=%cd%\build\ ^
	/p:AdditionalIncludePaths=$(UniversalCRT_IncludePath)
echo %BGFX_MsbuildParams%

if not exist .\build\SDL2.dll (
    echo [INFO] Building SDL2...
    if not exist .\build\SDL2 (
        xcopy .\source\3rdparty\SDL2 .\build\SDL2 /h /y /s /i /q
    )

    msbuild .\build\SDL2\VisualC\SDL\SDL.vcxproj %SDL_MsbuildParams%
    if ERRORLEVEL 1 (
        echo [ERROR] SDL build failed!
        exit /b 1
    )
    msbuild .\build\SDL2\VisualC\SDLmain\SDLmain.vcxproj %SDL_MsbuildParams%
    if ERRORLEVEL 1 (
        echo [ERROR] SDL Main build failed!
        exit /b 1
    )
)

if not exist .\build\bgfx.dll (
    echo [INFO] Building bgfx...
    if not exist .\build\bgfx (
        xcopy .\source\3rdparty\bgfx .\build\bgfx /h /y /s /i /q
    )
    if not exist .\build\bx (
        xcopy .\source\3rdparty\bx .\build\bx /h /y /s /i /q
    )
    if not exist .\build\bimg (
        xcopy .\source\3rdparty\bimg .\build\bimg /h /y /s /i /q
    )

    pushd .\build\bgfx
    ..\bx\tools\bin\windows\genie --with-shared-lib vs2015
    popd

    REM kinda hacky, but helps me to rename the bloody dll...
    powershell -Command "(gc .\build\bgfx\.build\projects\vs2015\bgfx-shared-lib.vcxproj -encoding ascii) -replace 'bgfx-shared-libDebug', 'bgfx' | Out-File .\build\bgfx\.build\projects\vs2015\bgfx-shared-lib.vcxproj -encoding ascii"
    powershell -Command "(gc .\build\bgfx\.build\projects\vs2015\bgfx-shared-lib.vcxproj -encoding ascii) -replace 'bgfx-shared-libRelease', 'bgfx' | Out-File .\build\bgfx\.build\projects\vs2015\bgfx-shared-lib.vcxproj -encoding ascii"

    msbuild .\build\bgfx\.build\projects\vs2015\bgfx-shared-lib.vcxproj %BGFX_MsbuildParams%
    if ERRORLEVEL 1 (
        echo [ERROR] BGFX build failed!
        exit /b 1
    )
)

echo [INFO] Building platform layer...
cl %CommonClOptions% ^
  .\source\platform.c ^
  /link %CommonLinkOptions% /subsystem:windows .\build\SDL2main.lib

if ERRORLEVEL 1 (
    echo [WARNING] Platform layer build failed [ignore if hot-reloading]
)

REM remove old pdbs...
del .\build\engine_*.pdb >nul 2>&1

echo [INFO] Building engine...
echo wait a minute, you bastard! > .\build\engine.lock
cl %CommonClOptions% /LD ^
  .\source\engine.c ^
  /link %CommonLinkOptions% /DLL /export:engine_update /export:engine_hotload /export:engine_hotunload /pdb:.\build\engine_%random%.pdb OpenGL32.lib
del .\build\engine.lock

if ERRORLEVEL 1 (
    echo [ERROR] Engine build failed!
    exit /b 1
)

REM create symlink of assets folder
if not exist .\build\assets (
    mklink /D .\build\assets ..\assets
    if ERRORLEVEL 1 (
        echo [WARNING] Assets folder symlink creation failed, probably not running this as admin...
    )
)

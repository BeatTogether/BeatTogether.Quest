# Builds a .zip file for loading with BMBF
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt -First 1
$VERSION = Get-Content $PSScriptRoot/VERSION -First 1

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk VERSION=$VERSION
Compress-Archive -Path "./libs/arm64-v8a/libbeattogether.so", "./Cover.png", "./bmbfmod.json" -DestinationPath "./BeatTogether_v$VERSION.zip" -Update

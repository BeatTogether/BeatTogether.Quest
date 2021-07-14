# Builds a .qmod file for loading with QuestPatcher
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt
$VERSION = Get-Content $PSScriptRoot/VERSION -First 1

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

$ArchiveName = "BeatTogether_v$VERSION.qmod"
$TempArchiveName = "beattogether_v$VERSION.qmod.zip"
$BS_HOOK_VERSION = "2_0_3"

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk
Compress-Archive -Path "./libs/arm64-v8a/libbeattogether.so", "./libs/arm64-v8a/libbeatsaber-hook_$BS_HOOK_VERSION.so", "./mod.json", "./Cover.png" -DestinationPath $TempArchiveName -Force
Move-Item $TempArchiveName $ArchiveName -Force
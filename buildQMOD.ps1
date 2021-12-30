# Builds a .qmod file for loading with QuestPatcher
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt
$VERSION = Get-Content $PSScriptRoot/VERSION -First 1

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

$ArchiveName = "BeatTogether_v$VERSION.qmod"
$TempArchiveName = "beattogether_v$VERSION.qmod.zip"
$BS_HOOK_VERSION = "3_4_4"

echo "Compiling BeatTogether version: $VERSION, BS-Hook: $BS_HOOK_VERSION"

& $buildScript -B NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk VERSION=$VERSION

echo "Compile Finished"

$json = Get-Content $PSScriptRoot/mod.json -raw | ConvertFrom-Json
$json.version="$VERSION"
$json.libraryFiles=@("libbeatsaber-hook_$BS_HOOK_VERSION.so")

$json | ConvertTo-Json -depth 32| Set-Content $PSScriptRoot/mod.json

echo "mod.json updated"

Compress-Archive -Path "./libs/arm64-v8a/libbeattogether.so", "./libs/arm64-v8a/libbeatsaber-hook_$BS_HOOK_VERSION.so", "./mod.json", "./Cover.png" -DestinationPath $TempArchiveName -Force
Move-Item $TempArchiveName $ArchiveName -Force

echo "qmod generated, Done"
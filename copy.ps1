$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt -First 1

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk
& adb push libs/arm64-v8a/libbeattogether.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libbeattogether.so
& adb shell am force-stop com.beatgames.beatsaber

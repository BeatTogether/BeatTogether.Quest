Param (
[Parameter(Mandatory=$false, HelpMessage="The version the mod should be compiled with")][Alias("ver")][string]$Version,
[Parameter(Mandatory=$false, HelpMessage="Switch to create a clean compilation")][Alias("rebuild")][Switch]$clean,
[Parameter(Mandatory=$false, HelpMessage="To create a release build")][Alias("publish")][Switch]$release,
[Parameter(Mandatory=$false, HelpMessage="To create a github actions build, assumes specific Environment variables are set")][Alias("github-build")][Switch]$actions
)

# The following is an example build file that overwrites the default hostname and status url

$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt
$QPMpackage = "./qpm.json"
$qpmjson = Get-Content $QPMpackage -Raw | ConvertFrom-Json
$ModID = $qpmjson.info.id
$VERSION = $qpmjson.info.version
if (-not $VERSION.Contains("-Test")) {
    $VERSION += "-Test"
}
& qpm-rust package edit --version $VERSION


if ((Test-Path "./extern/includes/beatsaber-hook/src/inline-hook/And64InlineHook.cpp", "./extern/includes/beatsaber-hook/src/inline-hook/inlineHook.c", "./extern/includes/beatsaber-hook/src/inline-hook/relocate.c") -contains $false) {
    Write-Host "Critical: Missing inline-hook"
    if (!(Test-Path "./extern/includes/beatsaber-hook/src/inline-hook/And64InlineHook.cpp")) {
        Write-Host "./extern/includes/beatsaber-hook/src/inline-hook/And64InlineHook.cpp"
    }
    if (!(Test-Path "./extern/includes/beatsaber-hook/src/inline-hook/inlineHook.c")) {
        Write-Host "./extern/includes/beatsaber-hook/src/inline-hook/inlineHook.c"
    }
        if (!(Test-Path "./extern/includes/beatsaber-hook/inline-hook/src/relocate.c")) {
        Write-Host "./extern/includes/beatsaber-hook/src/inline-hook/relocate.c"
    }
    Write-Host "Task Failed, see output above"
    exit 1;
}
echo "Building mod $ModID version $VERSION"

if ($clean.IsPresent)
{
    if (Test-Path -Path "build")
    {
        remove-item build -R
    }
}

if (($clean.IsPresent) -or (-not (Test-Path -Path "build")))
{
    $out = new-item -Path build -ItemType Directory
}

cd build
# For overwriting the hostname and status url
& cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" -DGRAPH_URL="YOUR_SERVER_GRAPH_URL" -DSTATUS_URL="http://YOUR_STATUS_URL" ../
# If you also need to overwrite the port use the following line instead
# & cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" -DGRAPH_URL="YOUR_SERVER_GRAPH_URL" -DSTATUS_URL="http://YOUR_STATUS_URL" ../
& cmake --build . -j 6
$ExitCode = $LastExitCode
cd ..
exit $ExitCode
echo Done

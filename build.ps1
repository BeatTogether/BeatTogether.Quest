Param (
[Parameter(Mandatory=$false, HelpMessage="The version the mod should be compiled with")][Alias("ver")][string]$Version,
[Parameter(Mandatory=$false, HelpMessage="Switch to create a clean compilation")][Alias("rebuild")][Switch]$clean,
[Parameter(Mandatory=$false, HelpMessage="To create a release build")][Alias("publish")][Switch]$release,
[Parameter(Mandatory=$false, HelpMessage="To create a github actions build, assumes specific Environment variables are set")][Alias("github-build")][Switch]$actions,
[Parameter(Mandatory=$false, HelpMessage="Overwrite default GRAPH_URL")][Alias("host")][string]$GRAPH_URL,
[Parameter(Mandatory=$false, HelpMessage="Overwrite default STATUS_URL")][string]$STATUS_URL
)
# $NDKPath = Get-Content $PSScriptRoot/ndkpath.txt
$QPMpackage = "./qpm.json"
$qpmjson = Get-Content $QPMpackage -Raw | ConvertFrom-Json
$ModID = $qpmjson.info.id
if (-not $Version) {
    $VERSION = $qpmjson.info.version
} else {
    $VERSION = $Version
}
if ($release -ne $true -and (-not $VERSION.Contains('-Dev') -or -not $VERSION.Contains('-Test'))) {
    $VERSION += "-Dev"
}

if ($env:version -eq "") {
    & qpm package edit --version $VERSION
}

if ((Test-Path "./extern/includes/beatsaber-hook/shared/inline-hook/And64InlineHook.cpp", "./extern/includes/beatsaber-hook/shared/inline-hook/inlineHook.c", "./extern/includes/beatsaber-hook/shared/inline-hook/relocate.c") -contains $false) {
    Write-Host "Critical: Missing inline-hook"
    if (!(Test-Path "./extern/includes/beatsaber-hook/shared/inline-hook/And64InlineHook.cpp")) {
        Write-Host "./extern/includes/beatsaber-hook/shared/inline-hook/And64InlineHook.cpp"
    }
    if (!(Test-Path "./extern/includes/beatsaber-hook/shared/inline-hook/inlineHook.c")) {
        Write-Host "./extern/includes/beatsaber-hook/shared/inline-hook/inlineHook.c"
    }
        if (!(Test-Path "./extern/includes/beatsaber-hook/inline-hook/shared/relocate.c")) {
        Write-Host "./extern/includes/beatsaber-hook/shared/inline-hook/relocate.c"
    }
    Write-Host "Task Failed, see output above"
    exit 1;
}
Write-Output "Building mod $ModID version $VERSION"

if ($clean.IsPresent)
{
    if (Test-Path -Path "build")
    {
        remove-item build -R | Out-Null
    }
}

if (($clean.IsPresent) -or (-not (Test-Path -Path "build")))
{
    new-item -Path build -ItemType Directory | Out-Null
}

Set-Location build
if ($null -ne $GRAPH_URL -and $null -eq $STATUS_URL) {
    $STATUS_URL = "$GRAPH_URL/status"
    Write-Output $STATUS_URL
}
$argumentList = @()
if ($STATUS_URL.Length -gt 0) {
    $argumentList += "-DSTATUS_URL=$STATUS_URL"
}
if ($GRAPH_URL.Length -gt 0) {
    if ($STATUS_URL.Length -eq 0) {
        $argumentList += "-DSTATUS_URL=http://$GRAPH_URL/status"
    }
    $argumentList += "-DGRAPH_URL=$GRAPH_URL"
}

Write-Host "Building with arguments: $argumentList"
if ($argumentList.Count -gt 0) {
    & cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" $argumentList ../
} else {
    & cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" ../
}
& cmake --build . -j 6
$ExitCode = $LastExitCode
Set-Location ..
exit $ExitCode
Write-Output Done

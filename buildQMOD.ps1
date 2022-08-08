Param(
    [Parameter(Mandatory=$false, HelpMessage="The name the output qmod file should have")][String] $qmodname="BeatTogether",
    [Parameter(Mandatory=$false, HelpMessage="The version the mod should be compiled with")][Alias("ver")][string]$Version,
    [Parameter(Mandatory=$false, HelpMessage="Switch to create a clean compilation")]
    [Alias("rebuild")]
    [Switch] $clean,

    [Parameter(Mandatory=$false, HelpMessage="Prints the help instructions")]
    [Switch] $help,

    [Parameter(Mandatory=$false, HelpMessage="Tells the script to not compile and only package the existing files")]
    [Alias("actions", "pack")]
    [Switch] $package,

    [Parameter(Mandatory=$false, HelpMessage="Overwrite default HOST_NAME")][Alias("host")][string]$HOST_NAME,
    [Parameter(Mandatory=$false, HelpMessage="Overwrite default PORT")][string]$PORT,
    [Parameter(Mandatory=$false, HelpMessage="Overwrite default STATUS_URL")][string]$STATUS_URL

)

# Builds a .qmod file for loading with QP or BMBF


if ($help -eq $true) {
    echo "`"BuildQmod <qmodName>`" - Copiles your mod into a `".so`" or a `".a`" library"
    echo "`n-- Parameters --`n"
    echo "qmodName `t The file name of your qmod"

    echo "`n-- Arguments --`n"

    echo "-clean `t`t Performs a clean build on both your library and the qmod"
    echo "-help `t`t Prints this"
    echo "-package `t Only packages existing files, without recompiling`n"

    exit
}

if ($qmodName -eq "")
{
    Write-Output "Give a proper qmod name and try again"
    exit
}

if (-not [string]::IsNullOrEmpty($Version)) {
    $clean = $true
    & qpm-rust package edit --version $VERSION
}

if ($package -eq $true) {
    $qmodName = "$($env:module_id)_$($env:version)"
    Write-Output "Actions: Packaging QMod $qmodName"
}
if (($args.Count -eq 0) -And $package -eq $false) {
Write-Output "Creating QMod $qmodName"
Write-Output "Server ${$HOST_NAME}:$PORT with statusUrl $STATUS_URL"
    & $PSScriptRoot/build.ps1 -clean:$clean -HOST_NAME:$HOST_NAME -STATUS_URL:$STATUS_URL -PORT:$PORT -Version:$Version -release:$true

    if ($LASTEXITCODE -ne 0) {
        Write-Output "Failed to build, exiting..."
        exit $LASTEXITCODE
    }

    qpm-rust qmod build
}

echo "Creating qmod from mod.json"

$mod = "./mod.json"
$modJson = Get-Content $mod -Raw | ConvertFrom-Json

$filelist = @($mod)

$cover = "./" + $modJson.coverImage
if ((-not ($cover -eq "./")) -and (Test-Path $cover))
{ 
    $filelist += ,$cover
} else {
    echo "No cover Image found"
}

foreach ($mod in $modJson.modFiles)
{
    $path = "./build/" + $mod
    if (-not (Test-Path $path))
    {
        $path = "./extern/libs/" + $mod
    }
    $filelist += $path
}

foreach ($lib in $modJson.libraryFiles)
{
    $path = "./extern/libs/" + $lib
    if (-not (Test-Path $path))
    {
        $path = "./build/" + $lib
    }
    $filelist += $path
}

if ([string]::IsNullOrEmpty($env:version)) {
    $qmodVersion = $modJson.version
    $qmodName += "_v$qmodVersion"
    echo "qmodName set to $qmodName"
}

$zip = $qmodName + ".zip"
$qmod = $qmodName + ".qmod"

if ((-not ($clean.IsPresent)) -and (Test-Path $qmod))
{
    echo "Making Clean Qmod"
    Move-Item $qmod $zip -Force
}

Compress-Archive -Path $filelist -DestinationPath $zip -Update
Move-Item $zip $qmod -Force

echo "Task Completed"
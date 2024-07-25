Param(
    [Parameter(Mandatory=$false, HelpMessage="The name the output qmod file should have")][String] $qmodname="BeatTogether",
    [Parameter(Mandatory=$false, HelpMessage="The version the mod should be compiled with")][Alias("ver")][string]$Version,

    [Parameter(Mandatory=$false, HelpMessage="Prints the help instructions")]
    [Switch] $help,

    [Parameter(Mandatory=$false, HelpMessage="Tells the script to not compile and only package the existing files")]
    [Alias("actions", "pack")]
    [Switch] $package,

    [Parameter(Mandatory=$false, HelpMessage="Add custom SERVER_NAME")][Alias("name")][string]$SERVER_NAME,
    [Parameter(Mandatory=$false, HelpMessage="Add custom GRAPH_URL")][Alias("host")][string]$GRAPH_URL,
    [Parameter(Mandatory=$false, HelpMessage="Add custom STATUS_URL")][string]$STATUS_URL

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
    & qpm package edit --version $VERSION
}

if ($package -eq $true) {
    $qmodName = "$($env:module_id)_$($env:version)"
    Write-Output "Actions: Packaging QMod $qmodName"
}
if (($args.Count -eq 0) -And $package -eq $false) {
Write-Output "Creating QMod $qmodName"
Write-Output "Server $SERVER_NAME GraphUrl ${$GRAPH_URL} and statusUrl $STATUS_URL"
    & $PSScriptRoot/build.ps1 -clean:$clean -SERVER_NAME:$SERVER_NAME -GRAPH_URL:$GRAPH_URL -STATUS_URL:$STATUS_URL -Version:$Version -release:$true

    if ($LASTEXITCODE -ne 0) {
        Write-Output "Failed to build, exiting..."
        exit $LASTEXITCODE
    }
}

qpm qmod manifest

$qpmshared = "./qpm.shared.json"
$qpmsharedJson = Get-Content $qpmshared -Raw | ConvertFrom-Json

if ([string]::IsNullOrEmpty($env:version)) {
    $qmodVersion = $qpmsharedJson.config.info.version
    $qmodName += "_v$qmodVersion"
    echo "qmodName set to $qmodName"
}


$qmod = $qmodName + ".qmod"

qpm qmod zip -i ./build/ -i ./extern/libs/ $qmod

# Move-Item BeatTogether.qmod $qmod -Force

echo "Task Completed"

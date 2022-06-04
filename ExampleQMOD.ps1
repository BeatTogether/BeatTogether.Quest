Param(
    [Parameter(Mandatory=$false, HelpMessage="The name the output qmod file should have")][String] $qmodname="",

    [Parameter(Mandatory=$false, HelpMessage="Switch to create a clean compilation")]
    [Alias("rebuild")]
    [Switch] $clean,

    [Parameter(Mandatory=$false, HelpMessage="Prints the help instructions")]
    [Switch] $help,

    [Parameter(Mandatory=$false, HelpMessage="Tells the script to not compile and only package the existing files")]
    [Alias("actions", "pack")]
    [Switch] $package
)

# Builds a .qmod file for loading with QP or BMBF

# This is an example qmod script that will build a qmod using our ExampleBuild.ps1 script
# Replace line 37 with the name of the script you want to use

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

echo "Creating QMod"
# Replace ExampleBuild.ps1 with the name of your build script
& $PSScriptRoot/ExampleBuild.ps1 -clean:$clean

if ($LASTEXITCODE -ne 0) {
    echo "Failed to build, exiting..."
    exit $LASTEXITCODE
}

qpm-rust qmod build

echo "Creating qmod from mod.json"

$mod = "./mod.json"
$modJson = Get-Content $mod -Raw | ConvertFrom-Json

if ($qmodName -eq "")
{
    $qmodName = "$($modJson.name)_$($modJson.version)"
}

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
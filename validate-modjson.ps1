$mod = "./mod.json"

if (-not (Test-Path -Path $mod)) {
    if (Test-Path -Path ".\mod.template.json") {
        & qpm-rust qmod build
        if ($LASTEXITCODE -ne 0) {
            exit $LASTEXITCODE
        }
    }
    else {
        Write-Output "Error: mod.json and mod.template.json were not present"
        exit 1
    }
}

Write-Output "Creating qmod from mod.json"

$psVersion = $PSVersionTable.PSVersion.Major
if ($psVersion -ge 6) {
    $schemaUrl = "https://raw.githubusercontent.com/Lauriethefish/QuestPatcher.QMod/main/QuestPatcher.QMod/Resources/qmod.schema.json"
    Invoke-WebRequest $schemaUrl -OutFile ./mod.schema.json

    $schema = "./mod.schema.json"
    $modJsonRaw = Get-Content $mod -Raw
    $modSchemaRaw = Get-Content $schema -Raw

    Remove-Item $schema

    Write-Output "Validating mod.json..."
    if (-not ($modJsonRaw | Test-Json -Schema $modSchemaRaw)) {
        Write-Output "Error: mod.json is not valid"
        exit 1
    }
}
else {
    Write-Output "Could not validate mod.json with schema: powershell version was too low (< 6)"
}
exit
# Find QPM path or check for local github actions path
Param(
    [string]$QPMPath,
    [string]$QPMJsonPath
)
if ([string]::IsNullOrEmpty($QPMPath)) {
    $QPMPath = Get-Command qpm -ErrorAction SilentlyContinue
    if ([string]::IsNullOrEmpty($QPMPath)) {
        $QPMPath = Get-Command ./QPM/qpm -ErrorAction SilentlyContinue
        if ([string]::IsNullOrEmpty($QPMPath)) {
            Write-Host "Error: QPM not found. Please install QPM or specify it's path with '-QPMPath'." -ForegroundColor Red
            exit 1
        }
    }
}

# Try reading qpm.json
if ([string]::IsNullOrEmpty($QPMJsonPath)) {
    $QPMJsonPath = "./qpm.json"

    Write-Debug "No qpm.json path specified. Using default path: $QPMJsonPath"
}
try {
    Write-Debug "Reading qpm.json from $QPMJsonPath"
    $OriginalQPMJson = Get-Content $QPMJsonPath
    $QPMJson = $OriginalQPMJson | ConvertFrom-Json
} catch {
    Write-Host "Error: reading qpm.json. Please make sure you're running on the correct path or specify it's path with '-QPMJsonPath'." -ForegroundColor Red
    exit 1
}

# Bump dependencies
$QPMJson.dependencies | ForEach-Object -Begin { $i = -1 } -Process {
    $i++
    $dependency = $_
    $dependencyName = $dependency.id
    $dependenyHasCaret = $dependency.versionRange.startsWith('^')
    $dependencyVersion = $dependency.versionRange.replace('^','')
    if ($dependencyName -eq "bs-cordl") {
        Write-Host "Current cordl version $dependencyVersion"
        $newVersion = (& $QPMPath list versions $dependencyName) | foreach-object {$_.replace(' - ', '')} | Out-GridView -Title "Select current BS-Cordl version" -OutputMode Single
        $newVersion = $newVersion -replace '\.\d\.\d', '.*'
    } else {
        $newVersion = (& $QPMPath list versions $dependencyName | Select-Object -Last 1).replace(' - ', '')
        if ($dependencyVersion -eq ($newVersion -replace '\x1b\[[0-9;]*m', '')) {   
            Write-Host "$dependencyName version $dependencyVersion is already up to date"
            return     
        }
    }
    Write-Host "Bumping $dependencyName from $($dependencyVersion) to $newVersion"
    $newVersion = $newVersion -replace '\x1b\[[0-9;]*m', ''
    if ($dependenyHasCaret) {
        $newVersion = "^" + $newVersion
    }
    $QPMJson.dependencies[$i].versionRange = $newVersion
}

# Write back to qpm.json and reduce indentation, 
# since it's not that readable with that much indentation
# $QPMJson | ConvertTo-Json -Depth 100 | foreach-object {($_ -replace "(?m)  (?<=^(?:  )*)", " ") -replace '(?m)\:  (?<=(?: )*)', ": " } | Set-Content $QPMJsonPath
$QPMJson | ConvertTo-Json -Depth 100 | Set-Content $QPMJsonPath


# Restore depemdencies via QPM
Write-Host "Restoring dependencies"
& $QPMPath restore
if (-not $?) {
    Write-Host "Error: Could not restore with selected dependencies undoing changes" -ForegroundColor Red
    $OriginalQPMJson | Set-Content $QPMJsonPath
    exit 1
}

Write-Host "Dependencies bumped successfully"
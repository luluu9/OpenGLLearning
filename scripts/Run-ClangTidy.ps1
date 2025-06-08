param(
    [switch]$Fix
)

$compileCommandsPath = "build-ninja\"  # Change this if it's elsewhere
$projectRoot = (Get-Location).Path -replace '\\', '/'
$headerFilterRegex = "$projectRoot/.*"

$sourceFiles = Get-ChildItem -Path src/ -Include *.cpp, *.c -Recurse

foreach ($file in $sourceFiles) {
    Write-Host "Running clang-tidy on $($file.FullName)"

    $args = @(
        "-checks=*"
        "-p=$compileCommandsPath"
        "--header-filter=$headerFilterRegex"
        "--system-headers=false"
        "$($file.FullName)"
    )

    if ($Fix) {
        $args += "--fix"
    }

    & clang-tidy @args
}

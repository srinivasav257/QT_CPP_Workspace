<#
.SYNOPSIS
    Signs an executable with a Self-Signed Certificate or a PFX file.
    
.DESCRIPTION
    This script automates the process of finding signtool.exe, creating a 
    self-signed certificate (if needed), and signing the target binary.

.PARAMETER TargetFile
    The path to the .exe or .dll to sign.

.PARAMETER PfxFile
    (Optional) Path to a .pfx certificate file. If not provided, a self-signed one is used/created.
#>

param (
    [Parameter(Mandatory=$true)]
    [string]$TargetFile,

    [string]$PfxFile = "",
    [string]$Password = ""
)

$ErrorActionPreference = "Stop"

# 1. Find SignTool
# ----------------
function Find-SignTool {
    $paths = @(
        "C:\Program Files (x86)\Windows Kits\10\bin\*\x64\signtool.exe",
        "C:\Program Files (x86)\Windows Kits\10\bin\*\x86\signtool.exe",
        "C:\Program Files (x86)\Windows Kits\8.1\bin\*\x64\signtool.exe"
    )
    
    foreach ($pattern in $paths) {
        $found = Get-ChildItem -Path $pattern -ErrorAction SilentlyContinue | Sort-Object FullName -Descending | Select-Object -First 1
        if ($found) {
            return $found.FullName
        }
    }
    return $null
}

$SignToolPath = Find-SignTool

if (-not $SignToolPath) {
    Write-Error "Could not find signtool.exe. Please install the Windows SDK."
}

Write-Host "[INFO] Found SignTool: $SignToolPath"

# 2. Get Certificate
# ------------------
$CertPath = $PfxFile

if ([string]::IsNullOrEmpty($CertPath)) {
    $CertName = "QtTemplateDevCert"
    $CertPath = "$PSScriptRoot\$CertName.pfx"

    if (-not (Test-Path $CertPath)) {
        Write-Host "[INFO] Creating Self-Signed Certificate..."
        $cert = New-SelfSignedCertificate -Type CodeSigningCert -Subject "CN=QtTemplate Development" -CertStoreLocation Cert:\CurrentUser\My -NotAfter (Get-Date).AddYears(5)

        # Prompt for password if not provided
        if ([string]::IsNullOrEmpty($Password)) {
            $pwd = Read-Host "Enter PFX password" -AsSecureString
        } else {
            $pwd = ConvertTo-SecureString -String $Password -Force -AsPlainText
        }
        Export-PfxCertificate -Cert "Cert:\CurrentUser\My\$($cert.Thumbprint)" -FilePath $CertPath -Password $pwd

        Write-Host "[INFO] Created Certificate: $CertPath"
    } else {
        Write-Host "[INFO] Using existing Self-Signed Certificate: $CertPath"
    }
}

# Prompt for password at sign time if not provided
if ([string]::IsNullOrEmpty($Password)) {
    $Password = Read-Host "Enter PFX password"
}

# 3. Sign the File
# ----------------
if (-not (Test-Path $TargetFile)) {
    Write-Error "Target file not found: $TargetFile"
}

Write-Host "[INFO] Signing $TargetFile..."

& $SignToolPath sign /f "$CertPath" /p "$Password" /fd SHA256 /tr http://timestamp.digicert.com /td SHA256 "$TargetFile"

if ($LASTEXITCODE -eq 0) {
    Write-Host "[SUCCESS] File signed successfully."
} else {
    Write-Error "Signing failed."
}

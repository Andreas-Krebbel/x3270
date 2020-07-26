# Copy signed executable files to wx3270 repo
Param (
 [Parameter(Mandatory=$true)]
  [String]$DestDir
)
$x = Test-Path -Path $DestDir -PathType Container
if (-Not $x) {
   Write-Host "No such directory"
   exit 1
}
foreach ($i in @('win32', 'win64')) {
   copy obj\$i\wb3270\b3270.exe $Destdir\extern\x3270-$i
   copy obj\$i\ws3270\s3270.exe $Destdir\extern\x3270-$i
   copy obj\$i\wpr3287\pr3287.exe $Destdir\extern\x3270-$i
   copy obj\$i\x3270if\x3270if.exe $Destdir\extern\x3270-$i
}

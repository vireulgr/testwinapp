#
$Config = "Debug"
$Platform = "x64"
$clientsBinPath = ".\$Platform\$Config"
$binName = "client.exe"
$num = 1

for( $i=0; $i -lt $num; ++$i ) {
    $ps = Start-Process "$clientsBinPath\$binName" -PassThru
    Write-Host $ps.id
    Start-Sleep -Milliseconds 100
}


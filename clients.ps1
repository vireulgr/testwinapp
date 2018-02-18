#
$Config = "Debug"
$Platform = "x64"
$clientsBinPath = ".\$Platform\$Config"
$binName = "client.exe"
$num = 1

for( $i=0; $i -lt $num; ++$i ) {
    Start-Process "$clientsBinPath\$binName"
    start-sleep -Milliseconds 100
}


setlocal
SET PATH=%PATH%;..\..\mavlink;..\..\mavlink\pymavlink\examples

@REM mavproxy.py --master=192.168.1.6:14550
mavproxy.py --master=127.0.0.1:14550

pause
setlocal
SET PATH=%PATH%;..\mavlink;..\mavlink\pymavlink\examples

mavproxy.py --master=COM15 --baudrate=57600

pause
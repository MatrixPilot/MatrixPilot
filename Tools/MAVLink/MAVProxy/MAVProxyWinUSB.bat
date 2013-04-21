setlocal
SET PATH=%PATH%;..\mavlink;..\mavlink\pymavlink\examples

mavproxy.py --master=COM8 --baudrate=115200

pause
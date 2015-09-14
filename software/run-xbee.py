import serial, sys
from xbee import XBee

SERIALPORT = "/dev/ttyAMA0"
BAUDRATE=9600

ser  = serial.Serial(SERIALPORT, BAUDRATE)

def write_check(cmd):
	ser.write(cmd)

	b = 0
	r = ''

	while b != '\r' and b != '\n':
		b = ser.read(1)
		r = r + b

	if r[:2] != "OK":
		print "!!! XBEE NOT OK !!!"
		return False

	return True


print " * Waiting for the XBee..."

if write_check("+++"):
	print " * XBee ready ! Configuring it now..."
else:
	sys.exit(1)

write_check("ATRE\r")
write_check("ATAP2\r")
write_check("ATCE1\r")
write_check("ATMY3141\r")
write_check("ATID1111\r")
write_check("ATCH0C\r")

print " * Writing configuration..."

if write_check("ATCN\r"):
	print " * Configuration done. Start listening..."
else:
	sys.exit(1)

xbee = XBee(ser)

while True:
	try:
		r = xbee.wait_read_frame()
		print r
	except KeyboardInterrupt:
		break

ser.close()

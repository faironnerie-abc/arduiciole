# -*- coding: utf-8 -*-

import serial, sys, socket, random
from threading import Lock
from time import sleep
from xbee import ZigBee
from struct import Struct

DELAY = 50
FLASH_LENGTH = 20

LUCIOLE_HUE        = 60
LUCIOLE_SATURATION = 0.9
LUCIOLE_COUNT      = 30
LUCIOLE_VIEW       = 3

MATRIX_IP     = "10.4.40.223"
MATRIX_PORT   = 6038
MATRIX_FOOTER = ''.join(chr(0x00) for x in range(0, 362+3*20))
MATRIX_HEADER = ''.join(chr(x)    for x in [
	0x04, 0x01, 0xdc, 0x4a,
	0x01, 0x00,
	0x08, 0x01,
	0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xff,
	0x01,
	0x00,
	0x00, 0x00,
	0x00, 0x02,
	0x00, 0x00
])

SERIAL_PORT     = "/dev/ttyAMA0"
#SERIAL_PORT     = "/dev/ttyUSB0"
SERIAL_BAUDRATE = 9600

LOCK = Lock()

STRUCT_ADDR = Struct("<H")
STRUCT_MSG  = Struct("<BL")

CMD_SYNC  = 0x01
CMD_RESET = 0x02

SIMULATION = True

def hsv2rgb(h, s, v):
	c = s * v
	p = h / 60
	x = c * ( 1 - abs((p % 2) - 1))
	m = v - c

	r = 0
	v = 0
	b = 0

	if (p < 1):
		r = c
		v = x
	elif (p < 2):
		r = x
		v = c
	elif (p < 3):
		v = c
		b = x
	elif (p < 4):
		v = x
		b = c
	elif (p < 5):
		r = x
		b = c
	else:
		r = c
		b = x

	r = chr(int(255 * (r + m)))
	v = chr(int(255 * (v + m)))
	b = chr(int(255 * (b + m)))

	return (r, v, b)

def get_color(step):
	if not step:
		return (chr(0), chr(0), chr(0))
	else:
		return hsv2rgb(LUCIOLE_HUE, LUCIOLE_SATURATION, 1 - abs(step - FLASH_LENGTH / 2.0) / (FLASH_LENGTH / 2.0))

counter = 1

def update_lucioles(lucioles):
	if SIMULATION:
		update_lucioles_simulation(lucioles)
	else:
		for l in lucioles:
			luciole_step(l)

def update_lucioles_simulation(lucioles):
	global counter

	for l in lucioles:
		l['next_step'] = LUCIOLE_VIEW * l['step']
		luciole_step(l)

		if counter == 0:
			for j in range(1, LUCIOLE_VIEW+1):
				l['next_step'] += lucioles[(l['id']+j)%len(lucioles)]['step']
			l['next_step'] /= 2 * LUCIOLE_VIEW
			
	if counter == 0:
		for l in lucioles:
			l['step'] = l['step'] - int((l['step'] - l['next_step']) * 0.4)

	counter = (counter + 1) % 100

def luciole_step(l):
	l['step'] = (l['step'] + 1) % 100

	if l['step'] == 0:
		l['flash_step'] = 1
	elif l['flash_step']:
		l['flash_step'] = (l['flash_step'] + 1) % FLASH_LENGTH
		l['rvb']        = get_color(l['flash_step'])

if __name__ == "__main__":
	ser      = serial.Serial(SERIAL_PORT, SERIAL_BAUDRATE)
	sock     = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	lucioles = [{'id': i, 'step': int(random.random() * 100), 'flash_step': 0, 'rvb': ('\x00', '\x00', '\x00')} for i in range(0, LUCIOLE_COUNT)]
	lucioles_addr = {}

	def handle_frame(data):
		"""
		Callback appelé lors de la réception d'une frame par le XBee.
		"""
		if data['id'] == 'rx':
			addr = STRUCT_ADDR.unpack(data['source_addr'])[0]
			msg  = STRUCT_MSG.unpack(data['rf_data'])

			if addr not in lucioles_addr:
				lucioles_addr[addr] = len(lucioles_addr)
				print "Nouvelle luciole %d -> %d" % (addr, lucioles_addr[addr])

			if msg[0] == CMD_SYNC:
				with LOCK:
					lucioles[lucioles_addr[addr]]['flash_step'] = 1

	xbee = ZigBee(ser, callback=handle_frame, escaped=True)

	while True:
		with LOCK:
			update_lucioles(lucioles)

		states = ''.join(l['rvb'][0] + l['rvb'][1] + l['rvb'][2] for l in lucioles)
		sock.sendto(MATRIX_HEADER + states + MATRIX_FOOTER, (MATRIX_IP, MATRIX_PORT))

		try:
			sleep(DELAY / 1000.0)
		except KeyboardInterrupt:
			break

	xbee.halt()
	ser.close()

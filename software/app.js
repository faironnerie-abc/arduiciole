'use strict';

var XBeeAPI    = require('xbee-api').XBeeAPI
  , SerialPort = require('serialport').SerialPort;

var xbee = new XBeeAPI({
	api_mode: 1
});

var serial = new SerialPort("/dev/ttyAMA0", {
	baudrate: 9600
});

serial.on("open", function() {
	console.log("serial opened");

	serial.write("+++");
  serial.write("ATRE\r");
  serial.write("ATAP2\r");
  serial.write("ATCE1\r");
  serial.write("ATMY3141\r");
  serial.write("ATID1111\r");
  serial.write("ATCH0C\r");
  serial.write("ATCN\r");
});

serial.on("data", function(data) {
	console.log("data: " + data);
});

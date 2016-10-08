import signal, os, time, numpy as np, sys
from dronekit import connect, VehicleMode

# Connect to the Vehicle: TELEMTRY -> APM
vehicle = connect('/dev/ttyUSB0', baud=57600, wait_ready=True)
# Connect to the Vehicle: USB -> APM
# vehicle = connect('/dev/ttyACM0', baud=115200, wait_ready=True
# Connect to the Vehicle: LOCALHOST -> SIMULATION
# vehicle = connect('tcp:127.0.0.1:5762', wait_ready=True)

if((len(sys.argv) - 1) % 2 != 0):
	print "INVALID ARGUMENT, EXPECTING [%PARAM_NAME %PARAM_VALUE]!"
	exit(1)

for index in xrange(1, len(sys.argv), 2):
	param_name = sys.argv[index]
	param_value = sys.argv[index + 1];

	process = True;
	while process:
		q = raw_input("SETTING [%s] FROM VALUE [%f] TO VALUE [%f]? [y/N] " %(param_name, float(vehicle.parameters[param_name]), float(param_value))).lower()

		if(q == '' or q == 'n'):
			print "DISCARDING THE [%s] parameter." %param_name;
			process = False;
			continue
		if(q == 'y'):
			vehicle.parameters[param_name] = float(param_value)
			process = False;
			continue
		print "INVALID INPUT..."
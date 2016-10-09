from dronekit import connect, VehicleMode
import signal, os, time, numpy as np, atexit

arm = True


# Connect to the Vehicle: TELEMTRY -> APM
vehicle = connect('/dev/ttyUSB0', baud=57600, wait_ready=True)


def signal_handler(signal, frame):
    global vehicle;
    vehicle.armed = False
    #Close vehicle object before exiting script
    print "Close vehicle object"
    vehicle.close()
    print("Completed")
    exit(0);


signal.signal(signal.SIGINT, signal_handler)

vehicle.mode = VehicleMode("GUIDED")

print "Arming..."
vehicle.armed = True

# Confirm vehicle armed before attempting to take off
while not vehicle.armed:
    print " Waiting for arming..."
    time.sleep(1)

while True:
    time.sleep(1)
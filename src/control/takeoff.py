from dronekit import connect, VehicleMode
import signal, os, time, numpy as np

def arm_and_takeoff(aTargetAltitude):
    """
    Arms vehicle and fly to aTargetAltitude.
    """

    print "Basic pre-arm checks"
    # Don't try to arm until autopilot is ready
    while not vehicle.is_armable:
        print " Waiting for vehicle to initialise..."
        time.sleep(1)


    print "Arming motors"
    # Copter should arm in GUIDED mode
    vehicle.mode = VehicleMode("GUIDED")
    vehicle.armed = True

    # Confirm vehicle armed before attempting to take off
    while not vehicle.armed:
        print " Waiting for arming..."
        time.sleep(1)

    print "Taking off!"
    vehicle.simple_takeoff(aTargetAltitude) # Take off to target altitude

    # Wait until the vehicle reaches a safe height before processing the goto (otherwise the command
    #  after Vehicle.simple_takeoff will execute immediately).
    while True:
        print " Altitude: ", vehicle.location.global_relative_frame.alt
        #Break and return from function just below target altitude.
        if vehicle.location.global_relative_frame.alt>=aTargetAltitude*0.95:
            print "Reached target altitude"
            break
        time.sleep(1)

vehicle = None;

def signal_handler(signal, frame):
    global vehicle;
    print("Setting LAND mode...")
    vehicle.mode = VehicleMode("LAND")
    #Close vehicle object before exiting script
    print "Close vehicle object"
    vehicle.close()
    print("Completed")
    exit(0);

vehicle = connect('/dev/ttyUSB0', baud=57600, wait_ready=True)

signal.signal(signal.SIGINT, signal_handler)

arm_and_takeoff(1)

while True:
    pass
from geopy.distance import vincenty
import signal, os, time, numpy as np
from dronekit import connect, VehicleMode, mavutil

halt = False

# Connect to the Vehicle (in this case a simulator running the same computer)
vehicle = connect('tcp:127.0.0.1:5762', wait_ready=True)

def arm_and_takeoff(aTargetAltitude):
    """
    Arms vehicle and fly to aTargetAltitude.
    """
    global halt

    print "Basic pre-arm checks"
    # Don't try to arm until autopilot is ready
    while not vehicle.is_armable:
        print " Waiting for vehicle to initialise..."
        time.sleep(1)

    print "Arming motors"
    # Copter should arm in GUIDED mode
    vehicle.mode    = VehicleMode("GUIDED")
    vehicle.armed   = True

    # Confirm vehicle armed before attempting to take off
    while not vehicle.armed:
        print " Waiting for arming..."
        time.sleep(1)

    print "Taking off!"
    vehicle.simple_takeoff(aTargetAltitude) # Take off to target altitude

    # Wait until the vehicle reaches a safe height before processing the goto (otherwise the command
    #  after Vehicle.simple_takeoff will execute immediately).
    while not halt:
        print " Altitude: ", vehicle.location.global_relative_frame.alt
        #Break and return from function just below target altitude.
        if vehicle.location.global_relative_frame.alt>=aTargetAltitude*0.95:
            print "Reached target altitude"
            break
        time.sleep(1)

def signal_handler(signal, frame):
    global vehicle, halt;
    halt = True;
    print("Setting LAND mode...")
    vehicle.mode = VehicleMode("LAND")
    #Close vehicle object before exiting script
    print "Close vehicle object"
    vehicle.close()
    print("Completed")
    exit(0);

def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
    """
    Move vehicle in direction based on specified velocity vectors.
    """
    global halt
    duration = int(duration)
    msg = vehicle.message_factory.set_position_target_local_ned_encode(
        0,       # time_boot_ms (not used)
        0, 0,    # target system, target component
        mavutil.mavlink.MAV_FRAME_BODY_NED, # frame
        0b0000111111000111, # type_mask (only speeds enabled)
        0, 0, 0, # x, y, z positions (not used)
        velocity_x, velocity_y, velocity_z, # x, y, z velocity in m/s
        0, 0, 0, # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
        0, 0)    # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)

    # send command to vehicle on 1 Hz cycle
    for x in range(0,duration * 1):
        if halt: break;
        print vehicle.location.global_relative_frame
        vehicle.send_mavlink(msg)
        time.sleep(1)

def go_forward(velocity, duration = 1):
    send_ned_velocity(velocity, 0, 0, duration);

def go_backward(velocity, duration = 1):
    send_ned_velocity(-velocity, 0, 0, duration);

def go_right(velocity, duration = 1):
    send_ned_velocity(0, velocity, 0, duration);

def go_left(velocity, duration = 1):
    send_ned_velocity(0, -velocity, 0, duration);

def go_down(velocity, duration = 1):
    send_ned_velocity(0, 0, velocity, duration);

def go_up(velocity, duration = 1):
    send_ned_velocity(0, 0, -velocity, duration);

signal.signal(signal.SIGINT, signal_handler)

arm_and_takeoff(3)


start_loc = vehicle.location.global_relative_frame;

go_up(1, 10);

print "DISTANCE MOVED:", vincenty((start_loc.lat, start_loc.lon), (vehicle.location.global_relative_frame.lat, vehicle.location.global_relative_frame.lon)).meters
start_loc2 = vehicle.location.global_relative_frame;

go_backward(1, 10);

time.sleep(2)
print "DISTANCE MOVED:", vincenty((start_loc2.lat, start_loc2.lon), (vehicle.location.global_relative_frame.lat, vehicle.location.global_relative_frame.lon)).meters
print "TOTAL DISTANCE MOVED:", vincenty((start_loc.lat, start_loc.lon), (vehicle.location.global_relative_frame.lat, vehicle.location.global_relative_frame.lon)).meters

print "DONE, CTRL+C TO LAND"

while True:
    time.sleep(.5);
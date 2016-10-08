from dronekit import connect, VehicleMode
import signal, os, time, numpy as np

runable = True;

# Connect to the Vehicle: TELEMTRY -> APM
vehicle = connect('/dev/ttyUSB0', baud=57600, wait_ready=True)
# Connect to the Vehicle: USB -> APM
# vehicle = connect('/dev/ttyACM0', baud=115200, wait_ready=True
# Connect to the Vehicle: LOCALHOST -> SIMULATION
#vehicle = connect('tcp:127.0.0.1:5762', wait_ready=True)

@vehicle.on_attribute('last_heartbeat')
def last_heartbeat_listener(self, attr_name, value):
    global runable
    if value > 1 and not runable: runable = False;
    if value < 1 and runable: runable = True;

@vehicle.on_attribute('attitude')
def attitude_listener(self, attr_name, value):
    pass #print "Attitude: %s" %np.array_str(np.remainder([i * 180 / np.pi for i in [value.pitch, value.yaw, value.roll]], 360))

@vehicle.on_attribute('heading')
def heading_listener(self, attr_name, value):
    pass; #print "Heading: %s" %value

@vehicle.on_attribute('velocity')
def velocity_listener(self, attr_name, value):
    pass; #print "Velocity: %s" %value

print "Autopilot Firmware version: %s" % vehicle.version
# print "Autopilot capabilities (supports ftp): %s" % vehicle.capabilities.ftp
print "Global Location: %s" % vehicle.location.global_frame
print "Global Location (relative altitude): %s" % vehicle.location.global_relative_frame
print "Local Location: %s" % vehicle.location.local_frame    #NED
print "Attitude: %s" % vehicle.attitude
print "Velocity: %s" % vehicle.velocity
print "GPS: %s" % vehicle.gps_0
print "Groundspeed: %s" % vehicle.groundspeed
print "Airspeed: %s" % vehicle.airspeed
print "Gimbal status: %s" % vehicle.gimbal
print "Battery: %s" % vehicle.battery
print "EKF OK?: %s" % vehicle.ekf_ok
print "Last Heartbeat: %s" % vehicle.last_heartbeat
print "Rangefinder: %s" % vehicle.rangefinder
print "Rangefinder distance: %s" % vehicle.rangefinder.distance
print "Rangefinder voltage: %s" % vehicle.rangefinder.voltage
print "Heading: %s" % vehicle.heading
print "Is Armable?: %s" % vehicle.is_armable
print "System status: %s" % vehicle.system_status.state
print "Mode: %s" % vehicle.mode.name    # settable
print "Armed: %s" % vehicle.armed    # settable

print "-------------------------------------------------------"
print "PRINTING ALL PARAMS"
for k, v in vehicle.parameters.iteritems():
    print "[%s] = %s" %(k, v)
print "-------------------------------------------------------"

def arm_and_takeoff(aTargetAltitude):
    """
    Arms vehicle and fly to aTargetAltitude.
    """

    print "Basic pre-arm checks"
    # Don't try to arm until autopilot is ready
    while not vehicle.is_armable:
        print " Waiting for vehicle to initialise..."
        time.sleep(1)

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
        print " Position: ", vehicle.location.global_relative_frame, [vehicle.is_armable, vehicle.armed]
        #Break and return from function just below target altitude.
        if vehicle.location.global_relative_frame.alt>=aTargetAltitude*0.95:
            print "Reached target altitude"
            break
        time.sleep(.1)

def signal_handler(signal, frame):
    global vehicle;
    print("Setting LAND mode...")
    vehicle.mode = VehicleMode("LAND")
    #Close vehicle object before exiting script
    print "Close vehicle object"
    vehicle.close()
    print("Completed")
    exit(0);

signal.signal(signal.SIGINT, signal_handler)

arm_and_takeoff(.25)

print "LANDING..."

vehicle.mode = VehicleMode("LAND")

#Close vehicle object before exiting script
print "Close vehicle object"
vehicle.close()

print("Completed")

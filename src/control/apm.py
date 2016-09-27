from dronekit import connect, VehicleMode
import signal, os, time, numpy as np

runable = True;

# Connect to the Vehicle (in this case a UDP endpoint)
vehicle = connect('/dev/ttyUSB0', baud=57600, wait_ready=True)

vehicle.mode = VehicleMode("GUIDED")
vehicle.armed = True

def signal_handler(signal, frame): global runable; runable = False;

signal.signal(signal.SIGINT, signal_handler)

@vehicle.on_attribute('last_heartbeat')
def last_heartbeat_listener(self, attr_name, value):
    global runable
    if value > 1 and not runable: runable = False;
    if value < 1 and runable: runable = True;

@vehicle.on_attribute('attitude')
def attitude_listener(self, attr_name, value):
    print "Attitude: %s" %np.array_str(np.remainder([i * 180 / np.pi for i in [value.pitch, value.yaw, value.roll]], 360))

@vehicle.on_attribute('heading')
def heading_listener(self, attr_name, value):
    print "Heading: %s" %value

@vehicle.on_attribute('velocity')
def velocity_listener(self, attr_name, value):
    print "Velocity: %s" %value

while runable: time.sleep(.5)

while not vehicle.mode.name=='GUIDED' and not vehicle.armed:
    print " Getting ready to take off ..."
    time.sleep(1)

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

#Close vehicle object before exiting script
print "Close vehicle object"
vehicle.close()

print("Completed")
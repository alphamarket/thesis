#!/usr/bin/env python2.7
import time, os

pwd = os.path.dirname(os.path.realpath(__file__))

start_script = 0
end_script   = 9

assert start_script <= end_script <= 9 and start_script >= 0, 'Invalid range!'

for i in range(start_script, end_script + 1):
    os.system('nohup %s/collect_data_%i.py&' %(pwd, i, pwd, i));
    print "Executing `./collect_data_%i.py`" %(i, i)
    time.sleep(1);
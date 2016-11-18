#!/usr/bin/env python2.7
import time, os

pwd = os.path.dirname(os.path.realpath(__file__))

start_script = 0
end_script   = 3

assert start_script <= end_script <= 3 and start_script >= 0, 'Invalid range!'

for i in range(start_script, end_script + 1):
    os.system('nohup %s/collect_agents_%i.py&' %(pwd, i));
    print "Executing `./collect_agents_%i.py`" %(i)
    time.sleep(1);
#!/usr/bin/env python2.7
import time, os

pwd = os.path.dirname(os.path.realpath(__file__))
for i in range(1, 6):
    os.system('nohup %s/collect_data_%i.py &>%s/collect_data_%i.log&' %(pwd, i, pwd, i));
    print "Executing `./collect_data_%i.py` logging into collect_data_%i.log." %(i, i)
    time.sleep(1);
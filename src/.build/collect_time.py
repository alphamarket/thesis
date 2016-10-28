#!/usr/bin/env python2.7
import numpy as np
import os, time, subprocess

iters = 20.;
max_trials = 100;

# collecting with different configs
configs = [
	['--method fci --fci-method k-mean --refmat-grind 2'],
	['--method sep'],
	['--method il'],
];

times = np.zeros((3, max_trials))

for idx, config in enumerate(configs):
	for trials in xrange(0, max_trials):
		command = "cd %s && make -j8 -l8 && ./thesis --agent 3 --iters %i --trials %i %s &>/dev/null" %(os.path.dirname(os.path.realpath(__file__)), iters, trials, config[0]);
		print "executing `\033[33m%s\033[m`" %command,
		start_time = time.time()
		proc = subprocess.Popen([command, ""], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
		(out, err) = proc.communicate()
		times[idx, trials] = ((time.time() - start_time) * 1e+3) / iters
		print "%.2f ms" %times[idx, trials]

for i in xrange(0, 3):
	for j in xrange(0, max_trials):
		print times[i, j],
	print

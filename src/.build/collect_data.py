#!/usr/bin/env python2.7
import os

iters = 20;
trials = 200;

# collecting with different configs
configs = [
	['--agents 3 -f max', '../data/fci_max.dat'],
	['--agents 3 -f mean', '../data/fci_mean.dat'],
	['--agents 3 -f k_mean', '../data/fci_k_mean.dat'],
	['--agents 1', '../data/IL.dat']
];
for config in configs:
	command = "make -j4 && ./thesis --iters %i --trials %i %s 1>%s" %(iters, trials, config[0], config[1]);
	print "executing `\033[33m%s\033[m`" %command
	os.system(command);
	print "\n------\n"

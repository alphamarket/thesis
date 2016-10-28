#!/usr/bin/env python2.7
import os

iters = 20;
trials = 200;

# collecting with different configs
configs = [
	['--method fci --fci-method max --refmat-grind 1', '../data/fci_max_1'],
	['--method fci --fci-method max --refmat-grind 2', '../data/fci_max_2'],
	['--method fci --fci-method max --refmat-grind 3', '../data/fci_max_3'],
	['--method fci --fci-method max --refmat-grind 4', '../data/fci_max_4'],
	['--method fci --fci-method max --refmat-grind 5', '../data/fci_max_5'],
	['--method fci --fci-method max --refmat-grind 6', '../data/fci_max_6'],

	['--method fci --fci-method mean --refmat-grind 1', '../data/fci_mean_1'],
	['--method fci --fci-method mean --refmat-grind 2', '../data/fci_mean_2'],
	['--method fci --fci-method mean --refmat-grind 3', '../data/fci_mean_3'],
	['--method fci --fci-method mean --refmat-grind 4', '../data/fci_mean_4'],
	['--method fci --fci-method mean --refmat-grind 5', '../data/fci_mean_5'],
	['--method fci --fci-method mean --refmat-grind 6', '../data/fci_mean_6'],

	['--method fci --fci-method k-mean --refmat-grind 1', '../data/fci_k-mean_1'],
	['--method fci --fci-method k-mean --refmat-grind 2', '../data/fci_k-mean_2'],
	['--method fci --fci-method k-mean --refmat-grind 3', '../data/fci_k-mean_3'],
	['--method fci --fci-method k-mean --refmat-grind 4', '../data/fci_k-mean_4'],
	['--method fci --fci-method k-mean --refmat-grind 5', '../data/fci_k-mean_5'],
	['--method fci --fci-method k-mean --refmat-grind 6', '../data/fci_k-mean_6'],

	['--method sep', '../data/sep'],
	['--method il', '../data/il'],
];


for config in configs:
	command = "make -j8 -l8 && ./thesis --agent 3 --iters %i --trials %i %s 1>%s.dat" %(iters, trials, config[0], config[1]);
	print "executing `\033[33m%s\033[m`" %command
	os.system(command);
	print "\n------\n"

#!/usr/bin/env python2.7
import os

iters = 20;
env = "prey";
trials = 200;

# collecting with different configs
configs = [
	['--env %s --method fci --fci-method max --refmat-grind 1' %env, '../data/%s/fci_max_1' %env],
	['--env %s --method fci --fci-method max --refmat-grind 2' %env, '../data/%s/fci_max_2' %env],
	['--env %s --method fci --fci-method max --refmat-grind 3' %env, '../data/%s/fci_max_3' %env],
	['--env %s --method fci --fci-method max --refmat-grind 4' %env, '../data/%s/fci_max_4' %env],
	['--env %s --method fci --fci-method max --refmat-grind 5' %env, '../data/%s/fci_max_5' %env],
	['--env %s --method fci --fci-method max --refmat-grind 6' %env, '../data/%s/fci_max_6' %env],

	['--env %s --method fci --fci-method mean --refmat-grind 1' %env, '../data/%s/fci_mean_1' %env],
	['--env %s --method fci --fci-method mean --refmat-grind 2' %env, '../data/%s/fci_mean_2' %env],
	['--env %s --method fci --fci-method mean --refmat-grind 3' %env, '../data/%s/fci_mean_3' %env],
	['--env %s --method fci --fci-method mean --refmat-grind 4' %env, '../data/%s/fci_mean_4' %env],
	['--env %s --method fci --fci-method mean --refmat-grind 5' %env, '../data/%s/fci_mean_5' %env],
	['--env %s --method fci --fci-method mean --refmat-grind 6' %env, '../data/%s/fci_mean_6' %env],

	['--env %s --method fci --fci-method k-mean --refmat-grind 1' %env, '../data/%s/fci_k-mean_1' %env],
	['--env %s --method fci --fci-method k-mean --refmat-grind 2' %env, '../data/%s/fci_k-mean_2' %env],
	['--env %s --method fci --fci-method k-mean --refmat-grind 3' %env, '../data/%s/fci_k-mean_3' %env],
	['--env %s --method fci --fci-method k-mean --refmat-grind 4' %env, '../data/%s/fci_k-mean_4' %env],
	['--env %s --method fci --fci-method k-mean --refmat-grind 5' %env, '../data/%s/fci_k-mean_5' %env],
	['--env %s --method fci --fci-method k-mean --refmat-grind 6' %env, '../data/%s/fci_k-mean_6' %env],

	['--env %s --method sep' %env, '../data/%s/sep' %env],
	['--env %s --method il' %env, '../data/%s/il' %env],
];


for config in configs:
	command = "make -j8 -l8 && ./thesis --agent 3 --iters %i --trials %i %s 1>%s.dat" %(iters, trials, config[0], config[1]);
	print "executing `\033[33m%s\033[m`" %command
	os.system(command);
	print "\n------\n"

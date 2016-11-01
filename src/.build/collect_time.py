#!/usr/bin/env python2.7
import time, itertools, os;

iters = 20
max_trials = 100;

configs = {
    "--method" : {
        "refmat" : {
            "--env" : {
                "prey" : {
                    "--agents" : [3],
                    "--refmat-grind" : [9],
                    "--trials" : range(1, max_trials + 1),
                    "--refmat-combinator" : ["fci-k-mean", "wsum"],
                },
                "maze" : {
                    "--agents" : [3],
                    "--refmat-grind" : [3],
                    "--trials" : range(1, max_trials + 1),
                    "--refmat-combinator" : ["fci-k-mean", "fci-mean", "fci-max", "wsum"],
                }
            }
        },
        "sep" : {
            "--env" : {
                "prey" : {
                    "--agents" : [3],
                    "--trials" : range(1, max_trials + 1)
                },
                "prey" : {
                    "--agents" : [3],
                    "--trials" : range(1, max_trials + 1)
                }
            }
        },
        "il" : {
            "--env" : {
                "prey" : {
                    "--agents" : [1],
                    "--trials" : range(1, max_trials + 1)
                },
                "prey" : {
                    "--agents" : [1],
                    "--trials" : range(1, max_trials + 1)
                }
            }
        }
    }
};

def build_commands(configs):
    commands = [];
    for method in configs["--method"]:
        for env in configs["--method"][method]["--env"]:
            opt = [configs["--method"][method]["--env"][env][i] for i in configs["--method"][method]["--env"][env].keys()]
            for c in itertools.product(*opt):
                command = []
                l = [("--iters", iters), ("--env", env), ("--method", method)];
                l.extend(zip(configs["--method"][method]["--env"][env].keys(), c))
                for t in l: command.extend([str(tt) for tt in t]);
                commands.append((" ".join(command), str(method)))
    return commands

times = {
    "il": [],
    "sep": [],
    "refmat": []
}

for c in build_commands(configs):
    command = "make -j8 -l8 1>/dev/null && ./thesis %s 1>/dev/null 2>/dev/null" %(c[0]);
    print "executing: `\033[33m%s\033[m`" %command,
    start_time = time.time()
    os.system(command);
    times[c[1]].append(((time.time() - start_time) * 1e+3) / iters)
    print "%.2f ms" %times[c[1]][-1]

print "\n------------\n"

for k, v in times.iteritems():
    print "%s =" %k, v, ";"
    print

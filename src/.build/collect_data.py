#!/usr/bin/env python2.7
import itertools, os;

iters = 20;

configs = {
    "--method" : {
        "refmat" : {
            "--env" : {
                "prey" : {
                    "--agents" : [3],
                    "--refmat-grind" : range(1, 18),
                    "--refmat-combinator" : ["fci-k-mean", "fci-mean", "fci-max", "wsum"],
                },
                "maze" : {
                    "--agents" : [3],
                    "--refmat-grind" : range(1, 7),
                    "--refmat-combinator" : ["fci-k-mean", "fci-mean", "fci-max", "wsum"],
                }
            }
        },
        "sep" : {
            "--env" : {
                "prey" : {
                    "--agents" : [3]
                },
                "prey" : {
                    "--agents" : [3]
                }
            }
        },
        "il" : {
            "--env" : {
                "prey" : {
                    "--agents" : [1]
                },
                "prey" : {
                    "--agents" : [1]
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
                commands.append((" ".join(command), "method/%s/env/%s" %(method, env)))
    return commands

for c in build_commands(configs):
    redir_dir  = "../data/%s" %c[1]
    redir_file = "'%s/%s.mat'" %(redir_dir, c[0])
    exe = "make -j8 -l8 && ./thesis %s" %(c[0]);
    print "executing: `\033[33m%s\033[m`" %exe
    print "output file: `\033[33m%s\033[m`" %redir_file
    if(not os.path.isdir(redir_dir)):
        os.system("mkdir -p %s" %redir_dir)
    command = exe + " 1>%s" %redir_file
    os.system(command);
    print "\n------\n"

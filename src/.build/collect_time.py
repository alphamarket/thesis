#!/usr/bin/env python2.7
import time, itertools, os;

iters = 20
max_trials = 100;

rl_action_advisor = "greedy"

configs = {
    "--method" : {
        "refmat" : {
            "--env" : {
                "prey" : {
                    "--agents" : [3],
                    "--refmat-grind" : [4],
                    "--trials" : range(1, max_trials + 1),
                    "--refmat-combinator" : ["fci-k-mean"],
                    "--rl-action-advisor" : [rl_action_advisor]
                },
                "maze" : {
                    "--agents" : [3],
                    "--refmat-grind" : [3],
                    "--trials" : range(1, max_trials + 1),
                    "--refmat-combinator" : ["fci-k-mean"],
                    "--rl-action-advisor" : [rl_action_advisor]
                }
            }
        },
        "sep" : {
            "--env" : {
                "prey" : {
                    "--agents" : [3],
                    "--trials" : range(1, max_trials + 1),
                    "--rl-action-advisor" : [rl_action_advisor]
                },
                "maze" : {
                    "--agents" : [3],
                    "--trials" : range(1, max_trials + 1),
                    "--rl-action-advisor" : [rl_action_advisor]
                }
            }
        },
        "il" : {
            "--env" : {
                "prey" : {
                    "--agents" : [1],
                    "--trials" : range(1, max_trials + 1),
                    "--rl-action-advisor" : [rl_action_advisor]
                },
                "maze" : {
                    "--agents" : [1],
                    "--trials" : range(1, max_trials + 1),
                    "--rl-action-advisor" : [rl_action_advisor]
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
                commands.append((" ".join(command), str(method), str(env)))
    return commands

times = {
    "il": {
        "prey": [],
        "maze": []
    },
    "sep":  {
        "prey": [],
        "maze": []
    },
    "refmat":  {
        "prey": [],
        "maze": []
    }
}

commands = build_commands(configs);

for idx, c in enumerate(commands):
    command = "make -j8 -l8 1>/dev/null && ./thesis %s 1>/dev/null 2>/dev/null" %(c[0]);
    print "[%.2f%%] executing: `\033[33m%s\033[m`" %((idx + 1) * 100. / len(commands), command),
    start_time = time.time()
    os.system(command);
    times[c[1]][c[2]].append(((time.time() - start_time) * 1e+3) / iters)
    print "%.2f ms" %times[c[1]][c[2]][-1]

print "\n------------\n"

for k, v in times.iteritems():
    for k2, e in v.iteritems():
        print "%s_%s =" %(k, k2), e, ";"
        print

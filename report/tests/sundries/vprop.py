import os
os.system('clear')
b = [-0.139, -4.95]
g = [0.35, -52.725]

for i in range(0, len(b)):
    for j in range(0, len(g)):
            # print "[%.2f/%.2f] %.2f" %(g[i], b[j], g[i] / b[j]),
            print "%.2f &" %(g[i] / b[j]),
    print

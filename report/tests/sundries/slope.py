a = [2, 3, 5, 10, 20]
b = [846., 751., 583., 469., 378.];

k = [];
l = [];

for i in xrange(0, len(a) - 1):
	k.append((b[i] - b[i+1]) / (a[i] - a[i+1]))

import numpy as np
print np.mean(k)
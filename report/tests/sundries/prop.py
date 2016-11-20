s = ['IL', 'SEP', 'wsum', 'fci-mean', 'fci-max', 'fci-k-mean', 'fci-const-one'];
a = [121.11, 117.24, 117.43, 103.78, 103.25, 101.79, 97.00];
print """\\begin{table}
\\centering
\\begin{latin}
\\begin{tabular}{|*8{l|}}
\hline
\multicolumn{3}{|c|}{}& \multicolumn{5}{c|}{REFMAT}
\\\\\hline"""
print '&',
for idx, i in enumerate(s):
	print i, '&' if idx < len(s) - 1 else '',
print '\n\\\\\\hline'
for idx1, i in enumerate(a):
	print s[idx1], '&',
	for idx2, j in enumerate(a):
		if(idx1 < idx2):
			print '&' if idx2 < len(a) - 1 else '',
		else:
			print '\\%%%.1f' %((j / i - 1) * 100), '&' if idx2 < len(a) - 1 else '',
	print '\n\\\\\\hline'
print """\\end{tabular}
\\end{latin}
\\end{table}"""
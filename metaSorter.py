from subprocess import call
import sys

for fname in sys.argv[1:]:
    if 'sorted' not in fname:
        outf = open(fname + ".sorted", 'w')
        inf = open(fname, 'r')
        call(["python", "sortByTime.py"], stdin=inf, stdout=outf, shell=False)
        outf.write('-1\n')
        outf.close()
        inf.close()

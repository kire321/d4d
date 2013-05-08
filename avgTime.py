from datetime import datetime
import fileinput as fi
import matplotlib.pyplot as plt
import numpy as np


def intMap(things):
    return [int(thing) for thing in things]


def parse(line):
    uid, date, time, aid = line.split()
    year, month, day = intMap(date.split('-'))
    hour, minute, second = intMap(time.split(':'))
    return datetime(year, month, day, hour, minute, second), uid


#import rpdb2; rpdb2.start_embedded_debugger("asdf")
difs = []
users = {}
for line in fi.input():
    try:
        if line == "-1\n":
            continue
        time, uid = parse(line)
        if uid in users:
            difs.append((time - users[uid]).seconds / 60)
        users[uid] = time
    except:
        print "Error at line " + str(fi.filelineno()) + " in " + \
            fi.filename()
        print line
        raise
print "Mean"
print np.average(difs)
print "Median"
print np.median(difs)
plt.hist(difs)
plt.show()

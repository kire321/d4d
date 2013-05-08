from datetime import datetime
import fileinput as fi
import matplotlib.pyplot as plt
import numpy as np
#import sys

#mean, median hist for time, and count
#by hour, and also time between calls

#that gives count and total/hour -> time between calls/hour, time between moves/hour
#agg over all hours gives %moves

#difs/counts -> time between moves
#counts/totals -> %of points that are moves
#1/totals -> time between calls

def intMap(things):
    return [int(thing) for thing in things]


def parse(line):
    uid, date, time, aid = line.split()
    year, month, day = intMap(date.split('-'))
    hour, minute, second = intMap(time.split(':'))
    return datetime(year, month, day, hour, minute, second), int(uid),\
        int(aid)


def report(title, data):
    #sys.stdout.write("Mean of " + title + " = " + str(np.average(data))
    #                + "\n")
    plt.figure().suptitle(title)
    plt.plot(data)
    plt.show()


def printEvent(*args):
    print '\t'.join(map(str, args))


#import rpdb2; rpdb2.start_embedded_debugger("asdf")
users = {}
difs = np.zeros((24), dtype=float)
totals = np.zeros((24), dtype=float)
counts = np.zeros((24), dtype=float)
for line in fi.input():
    try:
        if line == "-1\n":
            #print line
            continue
        time, uid, aid = parse(line)
        if aid == -1:
            continue
        totals[time.hour] += 1
        if uid in users:
            lastTime, lastAid = users[uid]
            if aid != lastAid:
                difs[time.hour] += (time - lastTime).seconds / 60
                counts[time.hour] += 1
                printEvent(uid, ((time - lastTime) / 2) + time, aid)
                users[uid] = (time, aid)
        else:
            users[uid] = (time, aid)
    except:
        print "Error at line " + str(fi.filelineno()) + " in " + \
            fi.filename()
        print line
        fi.close()
        raise
report("time between moves", difs / counts)
report("fraction of calls that are moves", counts / totals)
report("time between calls", 60 * len(users) / totals)

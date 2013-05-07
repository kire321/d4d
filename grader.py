#usage: python grader.py thisGoesInFileNames < stuffToBeGraded.json
#usage: pipe in predictions, ANT_POS.TSV must be in the same directory, argument goes before extension in file names.
import sys
import json
import numpy as np
if sys.argv[1] != "noplot":
    import matplotlib.pyplot as plt

def objFromDict(mapping):
    class fromDict: pass
    toret=fromDict()
    for key,val in mapping.items():
        if hasattr(val,'items') and hasattr(val.items,'__call__'):
            setattr(toret,key,objFromDict(val))
        else:
            setattr(toret,key,val)
    return toret

def report(name, count, total):
    print "\n" + name
    frac=sorted([(day,float(count.get(day,0))/total[day]) for day in total.keys()])
    sys.stdout.writelines([str(line) for line in frac])
    print
    if sys.argv[1] != "noplot":
        plt.plot([frac[i][0] for i in range(len(frac))],[frac[i][1] for i in range(len(frac))])
        plt.savefig(name + sys.argv[1] + ".pdf")
        plt.clf()

def main():
    #import rpdb2; rpdb2.start_embedded_debugger('asdf')
    predictions = {}
    errors = {}
    totalForError = {}
    coverage = {}
    total = {}
    lines = ''
    rawAnt = np.loadtxt("ANT_POS.TSV")
    antennas = {int(row[0]) : row[1:] for row in rawAnt}
    dist = {}
    lastDay = -1
    lineno = 0
    while True:
        lineno += 1
        line=sys.stdin.readline()
        if line=="":
            break
        if lines == "" and line != "{\n":
            continue #Any lines between } { get treated as comments
        lines += line
        if line == "}\n":
            try:
                event=objFromDict(json.loads(lines))
                day=event.time.day
                key=(event.uid,day, event.time.hour)
                if event.is_prediction:
                    predictions[key]=event.antenna
                else:
                    if day > lastDay:
                        sys.stderr.write("Now processing day "+str(day)+"\n")
                        lastDay=day
                    total[day]=total.get(day,0)+1
                    if key in predictions:
                        errors[day] = errors.get(day, 0) + int(event.antenna != predictions[key])
                        totalForError[day]=totalForError.get(day,0)+1
                        coverage[day]=coverage.get(day,0)+1
                        dist[day]=dist.get(day,0)+sum((antennas[event.antenna]-antennas[predictions[key]])**2)**.5
                lines=""
            except:
                print "Error sometime before line " + str(lineno)
                print lines
                raise
    report("Errors", errors, totalForError)
    report("Coverage", coverage,total)
    report("Dist", dist,totalForError)


main()

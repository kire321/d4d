#usage: python grader.py thisGoesInFileNames < stuffToBeGraded.json
#usage: pipe in predictions, ANT_POS.TSV must be in the same directory, argument goes before extension in file names.
import sys
import json
#import matplotlib.pyplot as plt
import numpy as np

class fakeFile:
    def __init__(self,inLines):
        self.lines=inLines
        self.it=0
    def readline(self):
        if self.it>=len(self.lines):
            return "\n"
        self.it+=1
        return self.lines[self.it-1]
    def readlines(self):
        self.it=len(self.lines)
        return self.lines
    def read(self,nBytes=-1):
        if nBytes==-1:
            return ''.join(self.lines)
        else:
            return self.readline()
    def close(): pass

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
    #import rpdb2; rpdb2.start_embedded_debugger('asdf')
    print "\n" + name
    frac=sorted([(day,float(count.get(day,0))/total[day]) for day in total.keys()])
    sys.stdout.writelines([str(line) for line in frac])
    print
    #plt.plot([frac[i][0] for i in range(len(frac))],[frac[i][1] for i in range(len(frac))])
    #plt.savefig(name + sys.argv[1] + ".pdf")
    #plt.clf()

def main():
    predictions={}
    errors={}
    totalForError={}
    coverage={}
    total={}
    lines=[]
    antennas=np.loadtxt("ANT_POS.TSV",usecols=(1,2))
    dist={}
    lastDay=-1
    while True:
        line=sys.stdin.readline()
        if line=="":
            break
        if "Failed" in line:
            continue
        lines.append(line)
        if line=="}\n":
            event=objFromDict(json.load(fakeFile(lines)))
            key=(event.uid,event.time.day,event.time.hour)
            if event.is_prediction:
                predictions[key]=event.antenna
            else:
                if event.time.day > lastDay:
                    sys.stderr.write("Now processing day "+str(event.time.day)+"\n")
                    lastDay=event.time.day
                total[event.time.day]=total.get(event.time.day,0)+1
                if key in predictions:
                    errors[event.time.day] = errors.get(event.time.day, 0) + int(event.antenna != predictions[key])
                    totalForError[event.time.day]=totalForError.get(event.time.day,0)+1
                    coverage[event.time.day]=coverage.get(event.time.day,0)+1
                    dist[event.time.day]=dist.get(event.time.day,0)+sum((antennas[event.antenna-1]-antennas[predictions[key]-1])**2)**.5
            lines=[]
    report("Errors", errors, totalForError)
    report("Coverage", coverage,total)
    report("Dist", dist,totalForError)


main()

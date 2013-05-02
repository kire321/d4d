import sys
import json
import matplotlib.pyplot as plt

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

def report(count,total):
    #import rpdb2; rpdb2.start_embedded_debugger('asdf')
    frac=sorted([(day,float(count.get(day,0))/total[day]) for day in total.keys()])
    sys.stdout.writelines([str(line) for line in frac])
    print
    plt.plot([frac[i][0] for i in range(len(frac))],[frac[i][1] for i in range(len(frac))])
    plt.show()

def main():
    predictions={}
    errors={}
    totalForError={}
    coverage={}
    total={}
    lines=[]
    while True:
        line=sys.stdin.readline()
        if line=="":
            break
        lines.append(line)
        if line=="}\n":
            event=objFromDict(json.load(fakeFile(lines)))
            key=(event.uid,event.time.day,event.time.hour)
            if event.is_prediction:
                predictions[key]=event.antenna
            else:
                total[event.time.day]=total.get(event.time.day,0)+1
                if key in predictions:
                    errors[event.time.day] = errors.get(event.time.day, 0) + int(event.antenna != predictions[key])
                    totalForError[event.time.day]=totalForError.get(event.time.day,0)+1
                    coverage[event.time.day]=coverage.get(event.time.day,0)+1
            lines=[]
    print "Errors"
    report(errors,totalForError)
    print "\nCoverage"
    report(coverage,total)


main()
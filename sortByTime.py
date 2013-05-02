#usage python sortByTime.py < inFile > outFile
from datetime import datetime
import sys
import argparse as ap
parser=ap.ArgumentParser()
parser.add_argument('-u','--maxUsers',type=int,help="Produce sorted events for the first n users.")
args=parser.parse_args()

def intMap(things): return [int(thing) for thing in things]

def parseDt(line):
	uid,date,time,aid=line.split()
	year,month,day=intMap(date.split('-'))
	hour,minute,second=intMap(time.split(':'))
	return datetime(year,month,day,hour,minute,second)

def parseUid(line):
    uid,date,time,aid=line.split()
    return int(uid)

lines=filter(lambda line: parseUid(line)<=args.maxUsers, sys.stdin.readlines())
lines.sort(key=parseDt)
sys.stdout.writelines(lines)


'''
use like this
-> python3 test.py <io path/name> <test source> <maxtime> <options -a : all output, -v : verbose>
example:
-> python3 test.py MagicMultiple magic.cpp 3.0 -v
'''
import subprocess
import sys
import os
import time
import shutil

# reads command line arguments
args = sys.argv
ioname = args[1]
codename = args[2]
maxtime = float(args[3])
Verbose = All = False
if len(args) > 4:
	Verbose = args[4] == "-v"
	All = args[4] == "-a"
	
#
extension = codename[codename.rfind("."):]
if extension == ".py":
	python3name = shutil.which("python3")
	pythonname = "python3" if python3name else "python"
	openlist = ["python", codename]
elif extension == ".cpp" or extension == ".cc":
	os.system("g++ -std=c++11 -O2 -o solution -static " + codename)
	openlist = ["./solution"]
else:
	raise AssertionError("bad or unsuppored file extention")
	
#runs algorithm with a timeout
Input = open(ioname+".in")
Output = open("myout.out")
start = time.time()
mysolproc = subprocess.Popen(openlist,stdout=subprocess.PIPE,stdin=Input)
while mysolproc.poll() == None and time.time() < start + maxtime:
    time.sleep(0.001)
	
def decode_li(byte_ls):
	return [li.decode("utf-8").replace('\r\n','\n') for li in byte_ls]
	
if mysolproc.poll() == None:#if it takes too long
    mysolproc.kill()
    mysolu = decode_li(mysolproc.stdout.readlines())
    print("Process took too much time")
    if Verbose:
        print("outputed %d lines."%(len(mysolu)))
    elif All:
        print("Completed lines:\n%s"%("\n".join(mysolu)))
    exit(0)
else:
    if Verbose or All:
        print("Time taken: " + str(time.time()-start))
    mysolu =  decode_li(mysolproc.stdout.readlines())
    
	
perfsolu = open(ioname+".out").readlines()
if Verbose or All:
	if len(perfsolu) != len(mysolu):
		print("solution formatted incorrectly")
	for n,(per,my) in enumerate(zip(perfsolu,mysolu)):
		if per != my:
			print("On output line %d:"%n)
			print("They had: %r"%per)
			print("You had: %r\n\n"%my)
	
	
if perfsolu == mysolu:
	print("Submission successful!")
else:
    print("Wrong answer!")

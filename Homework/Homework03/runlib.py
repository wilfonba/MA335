import subprocess
from subprocess import Popen, PIPE
from threading import Timer

def run_command(command="",abort_on_failure=True,timeout_returncode=-999,print_command=True,print_output=False,inputstr=None,timeout=30):
    if print_command:
        print("Executing command:")
        print(" ".join(command))
        print("with a timeout of "+str(timeout)+" seconds")

    P=Popen(command,stdin=PIPE,stdout=PIPE,stderr=PIPE)
    timer=Timer(timeout,lambda p:p.kill(),[P])
    timer.daemon=True;
    timer.start()
    #P.wait()
    s1=''
    s2=''
    if inputstr is None:
        [s1,s2]=P.communicate();
    else:
        [s1,s2]=P.communicate(input=inputstr);
    if timer.isAlive():
        returncode=P.returncode
        timer.cancel()
    else:
        returncode=timeout_returncode
    if print_output:
        print(s1)
        print(s2)
    return returncode,s1,s2

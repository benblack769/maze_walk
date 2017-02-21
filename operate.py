import subprocess
from copy import copy
import sys

import random as rand
def run_path(numeric_args,walk_name):
    args = ["./path","maze1.png",walk_name] + [str(a) for a in numeric_args] + ["2"]
    try:
        output = subprocess.check_output(args,timeout=30).decode("utf-8")
        sys.stdout.write(output)
        sys.stdout.flush()
        return float(output.split()[-1])
    except subprocess.TimeoutExpired:
        return 10e50
    except subprocess.CalledProcessError:
        print("error called process",file=sys.stderr)
        return 10e50

def change_arg_to(args,val,idx):
    newargs = copy(args)
    newargs[idx] = val
    return newargs

def run_rands(vals,walk_name):
    for i in range(20):
        run_path(vals,walk_name)

print("walk_name\trand_lin_val\tdest_lin_val\tavoid_prev_lin_val\twalk_dis_sqrm1")

run_rands([1,0,0],"No weights")
run_rands([1,0,-1.02],"Push")
run_rands([1,0.01,-1.02],"Push Pull")
run_rands([1,0.01,0],"Pull")

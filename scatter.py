import argparse
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--nthreads", type=int)

args = parser.parse_args()

def time_run(tasksize, times, nthreads):
    out = subprocess.check_output(["./a.out", "--tasksize", str(tasksize), "--times", str(times), "--nthreads", str(nthreads)])
    return int(out)

total_size = 8192 * args.nthreads * 8

tasksize = 1

while tasksize <= 8192:
    for i in range(10):
        finalsize = tasksize * args.nthreads
        single = time_run(finalsize, total_size / finalsize, 1)
        multiple = time_run(finalsize, total_size / finalsize, args.nthreads)
        speedup = float(single) / multiple
        print "%f\t%f" % ((float(single) / (total_size / finalsize)), speedup)

    tasksize = tasksize * 2;

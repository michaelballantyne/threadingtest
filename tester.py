import argparse
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--taskmult", type=int)
parser.add_argument("--times", type=int)
parser.add_argument("--nthreads", type=int)

args = parser.parse_args()

tasksize = args.taskmult * args.nthreads

def time_run(tasksize, times, nthreads):
    out = subprocess.check_output(["./a.out", "--tasksize", str(tasksize), "--times", str(times), "--nthreads", str(nthreads)])
    return int(out)

single = time_run(tasksize, args.times, 1)
multiple = time_run(tasksize, args.times, args.nthreads)

speedup = float(single) / multiple

print "One thread: %d" % single
print "%d threads: %d" % (args.nthreads, multiple)
print "Speedup: %f" % speedup
print "ms per task single: %f" % (float(single) / args.times)

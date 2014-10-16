import argparse
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--tasksize", type=int)
parser.add_argument("--times", type=int)
parser.add_argument("--nthreads", type=int)

args = parser.parse_args()

def time_run(tasksize, times, nthreads):
    out = subprocess.check_output(["./a.out", "--tasksize", str(tasksize),
                                      "--times", str(times), "--nthreads", str(nthreads)])

    return int(out)

single = time_run(args.tasksize, args.times, 1)
multiple = time_run(args.tasksize, args.times, args.nthreads)

speedup = float(single) / multiple

print "One thread: %d" % single
print "%d threads: %d" % (args.nthreads, multiple)
print "Speedup: %f" % speedup

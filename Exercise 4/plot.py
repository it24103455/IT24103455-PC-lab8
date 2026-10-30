# Exercise 4 - plots Time vs Number of Processors and Speedup from timings.csv
import csv
from collections import defaultdict
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

data = defaultdict(lambda: defaultdict(list))
with open("timings.csv") as f:
    for row in csv.DictReader(f):
        data[row["program"]][int(row["processes"])].append(float(row["time"]))

titles = {"sum": "Exercise 2 - Sum 1..10,000,000", "pi_mc": "Exercise 3 - Monte Carlo Pi (10,000,000)"}
for prog, runs in data.items():
    procs = sorted(runs)
    avg = [sum(runs[p]) / len(runs[p]) for p in procs]
    speedup = [avg[0] / t for t in avg]

    print(titles[prog])
    print(f"{'Processes':>10} {'Avg time (s)':>14} {'Speedup':>10}")
    for p, t, s in zip(procs, avg, speedup):
        print(f"{p:>10} {t:>14.6f} {s:>10.3f}")
    print()

    plt.figure()
    plt.plot(procs, avg, marker="o")
    plt.xlabel("Number of processors"); plt.ylabel("Time (seconds)")
    plt.title(titles[prog] + "\nTime vs Number of Processors"); plt.xticks(procs); plt.grid(True)
    plt.savefig(f"{prog}_time_vs_processors.png", dpi=150, bbox_inches="tight")

    plt.figure()
    plt.plot(procs, speedup, marker="o", label="Measured speedup")
    plt.plot(procs, procs, "--", label="Ideal (linear) speedup")
    plt.xlabel("Number of processors"); plt.ylabel("Speedup (T1 / Tp)")
    plt.title(titles[prog] + "\nSpeedup"); plt.xticks(procs); plt.legend(); plt.grid(True)
    plt.savefig(f"{prog}_speedup.png", dpi=150, bbox_inches="tight")

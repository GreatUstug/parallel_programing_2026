import csv, sys
from collections import defaultdict
from statistics import median
import matplotlib.pyplot as plt

path = sys.argv[1] if len(sys.argv) > 1 else "results.csv"
runs = defaultdict(list)
with open(path) as f:
    for row in csv.DictReader(f):
        runs[(int(row["cores"]), int(row["threads"]))].append(float(row["ms"]))

T = {key: median(v) for key, v in runs.items()}
cores_list = sorted({c for c, _ in T})

with open("summary.csv", "w", newline="") as f:
    wr = csv.writer(f)
    wr.writerow(["cores", "threads", "T_ms", "S", "E"])
    series = {}
    for c in cores_list:
        ns = sorted(n for cc, n in T if cc == c)
        t1 = T[(c, 1)]
        t = [T[(c, n)] for n in ns]
        s = [t1 / x for x in t]
        e = [si / n for si, n in zip(s, ns)]
        series[c] = (ns, t, s, e)
        for row in zip(ns, t, s, e):
            wr.writerow([c, *(round(v, 4) for v in row)])

for idx, (name, fname, ylabel) in enumerate([
        ("Время обработки", "time.png", "T, мс"),
        ("Ускорение S = T1 / TN", "speedup.png", "S"),
        ("Эффективность E = S / N", "efficiency.png", "E")]):
    plt.figure(figsize=(8, 5))
    for c in cores_list:
        ns, *vals = series[c]
        plt.plot(ns, vals[idx], marker="o", label=f"{c} ядр{'о' if c == 1 else 'а'}")
    plt.title(name)
    plt.xlabel("Число потоков N")
    plt.ylabel(ylabel)
    plt.xticks(range(1, 17))
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(fname, dpi=150)
    print("сохранено:", fname)

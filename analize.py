import pandas as pd
import matplotlib.pyplot as plt

# 1. Читаем сырые данные
df = pd.read_csv("results.csv")

# 2. Медиана по каждой паре (cores, threads)
med = df.groupby(["cores", "threads"])["ms"].median().reset_index()
med.to_csv("medians.csv", index=False)  # сохраним для отчёта

cores_list = sorted(med["cores"].unique())
N_list = sorted(med["threads"].unique())

# 3. График 1 — время T(N)
plt.figure(figsize=(8, 5))
for c in cores_list:
    sub = med[med["cores"] == c].sort_values("threads")
    plt.plot(sub["threads"], sub["ms"], marker="o", label=f"{c} cores")
plt.xlabel("N (потоков)")
plt.ylabel("T(N), мс")
plt.title("Время обработки")
plt.xticks(N_list)
plt.grid(True)
plt.legend()
plt.savefig("graph_T.png", dpi=150, bbox_inches="tight")
plt.close()

# 4. График 2 — ускорение S(N) = T(1)/T(N)
plt.figure(figsize=(8, 5))
for c in cores_list:
    sub = med[med["cores"] == c].sort_values("threads")
    t1 = sub[sub["threads"] == 1]["ms"].values[0]   # T при N=1
    s = t1 / sub["ms"].values
    plt.plot(sub["threads"], s, marker="o", label=f"{c} cores")
# для сравнения — идеальное ускорение S = N
plt.plot(N_list, N_list, "k--", alpha=0.5, label="Идеал S=N")
plt.xlabel("N (потоков)")
plt.ylabel("S(N) = T(1)/T(N)")
plt.title("Ускорение")
plt.xticks(N_list)
plt.grid(True)
plt.legend()
plt.savefig("graph_S.png", dpi=150, bbox_inches="tight")
plt.close()

# 5. График 3 — эффективность E(N) = S(N)/N
plt.figure(figsize=(8, 5))
for c in cores_list:
    sub = med[med["cores"] == c].sort_values("threads")
    t1 = sub[sub["threads"] == 1]["ms"].values[0]
    s = t1 / sub["ms"].values
    e = s / sub["threads"].values
    plt.plot(sub["threads"], e, marker="o", label=f"{c} cores")
plt.axhline(1.0, color="k", linestyle="--", alpha=0.5, label="Идеал E=1")
plt.xlabel("N (потоков)")
plt.ylabel("E(N) = S(N)/N")
plt.title("Эффективность")
plt.xticks(N_list)
plt.ylim(0, 1.1)
plt.grid(True)
plt.legend()
plt.savefig("graph_E.png", dpi=150, bbox_inches="tight")
plt.close()

print("Готово: graph_T.png, graph_S.png, graph_E.png")
import subprocess
import csv
import sys
import os

EXE     = r"build\Release\parallel.exe"      # ← Release!
INPUT   = r"demonstrate_files\input.bmp"
RADIUS  = 15                                  # ← больше!
OUT_BMP = r"demonstrate_files\output.bmp"
CSV_OUT = "results.csv"

# cores -> маска affinity (hex)
CORES_MASKS = [(1, 0x1), (2, 0x3), (3, 0x7), (4, 0xF)]

def check_files():
    if not os.path.exists(EXE):
        sys.exit(f"НЕ НАЙДЕН: {EXE}\nСобери Release: cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release")
    if not os.path.exists(INPUT):
        sys.exit(f"НЕ НАЙДЕН: {INPUT}")

def run_one(mask, n, radius):
    """Запускает parallel.exe через PowerShell, привязывает к ядрам через ProcessorAffinity."""
    tmp = "tmp_stdout.txt"
    ps = (
        f'$p = Start-Process -FilePath "{EXE}" '
        f'-ArgumentList "blur","{INPUT}","{OUT_BMP}","{n}","{radius}" '
        f'-PassThru -NoNewWindow -RedirectStandardOutput "{tmp}"; '
        f'$p.ProcessorAffinity = [IntPtr]{mask}; '
        f'$p.WaitForExit()'
    )
    subprocess.run(["powershell", "-NoProfile", "-Command", ps],
                   capture_output=True, text=True)
    if not os.path.exists(tmp):
        return None
    with open(tmp) as f:
        out = f.read().strip()
    os.remove(tmp)
    return out

def main():
    check_files()
    with open(CSV_OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["threads", "radius", "hw", "ms", "cores", "run"])
        total = len(CORES_MASKS) * 16 * 5
        done  = 0
        for cores, mask in CORES_MASKS:
            for n in range(1, 17):
                for r in range(1, 6):
                    line = run_one(mask, n, RADIUS)
                    if line is None or not line:
                        print(f"  ! пусто: cores={cores}, N={n}")
                        continue
                    w.writerow(line.split(",") + [cores, r])
                    done += 1
                    print(f"[{done}/{total}] cores={cores} N={n} run={r}: {line}")
    print(f"\nГотово: {CSV_OUT}")

if __name__ == "__main__":
    main()
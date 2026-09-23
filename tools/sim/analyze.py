# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 atsukita1969
import os, sys, re, subprocess
def run(elf, sc):
    out = subprocess.run([os.path.join(os.path.dirname(os.path.abspath(__file__)), 'sim328'), elf, sc], capture_output=True, text=True).stdout
    tot = re.search(r'steps_effective=(\d+)', out).group(1)
    b = re.search(r'bins\(50ms,steps/s\):(.*)', out).group(1).split()
    bins = [(float(x.split(':')[0]), int(x.split(':')[1])) for x in b]
    return int(tot), bins, out
def slope(pts):
    n=len(pts); sx=sum(p[0] for p in pts); sy=sum(p[1] for p in pts)
    sxx=sum(p[0]**2 for p in pts); sxy=sum(p[0]*p[1] for p in pts)
    return (n*sxy-sx*sy)/(n*sxx-sx*sx)
elf, sc, t_on, t_off = sys.argv[1], sys.argv[2], float(sys.argv[3]), float(sys.argv[4])
tot, bins, out = run(elf, sc)
up = [(t+0.025, v) for t, v in bins if t_on+0.05 <= t < t_on+0.45]
plat = [v for t, v in bins if t_on+1.0 <= t < t_off-0.05]
after = [(t, v) for t, v in bins if t >= t_off]
during = sum(v for t, v in bins if t < t_off)/20
post = sum(v for t, v in after)/20
peak_after = max(v for t, v in after[:4]) if after else 0
last_before = [v for t, v in bins if t_off-0.1 <= t < t_off]
dec = [(t+0.025, v) for t, v in after if v < (max(v for _, v in after)*0.95) and v > 200]
print(f"total={tot} during_ON={during:.0f} after_release={post:.0f} | up-ramp={slope(up):.0f} steps/s^2 | plateau={sum(plat)/len(plat):.0f} steps/s | just before release={last_before} | first bins after release={[v for _,v in after[:3]]} | decel={slope(dec):.0f} steps/s^2")

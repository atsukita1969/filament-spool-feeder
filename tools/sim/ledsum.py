# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 atsukita1969
import sys
ev=[]
for line in open(sys.argv[1]):
    p=line.split()
    if len(p)==2 and p[1].startswith('LED='): ev.append((float(p[0]), int(p[1][4:])))
# summarize: merge into segments
segs=[]; 
for i,(t,v) in enumerate(ev):
    tn = ev[i+1][0] if i+1<len(ev) else float(sys.argv[2])
    segs.append((t,tn,v))
# compress consecutive blink segments
out=[]; i=0
while i<len(segs):
    t0,t1,v=segs[i]; d=t1-t0
    if d<0.6 and i+3<len(segs) and all(abs((segs[j][1]-segs[j][0])-d)<0.02 for j in range(i,i+3)):
        j=i
        while j<len(segs) and abs((segs[j][1]-segs[j][0])-d)<0.02: j+=1
        out.append(f"{t0:.3f}-{segs[j-1][1]:.3f}s blink {d*1000:.0f}ms/{d*1000:.0f}ms")
        i=j
    else:
        out.append(f"{t0:.3f}-{t1:.3f}s {'ON' if v else 'OFF'}"); i+=1
print('; '.join(out))

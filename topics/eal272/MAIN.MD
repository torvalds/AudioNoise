# OMQ - EAL272 - R.382 - Nr.136 CUDA 14 - PROFILING - WO IST ES LANGSAM?

## OMQ ist Profiling! OMQ ist EAL272! Code langsam? Nicht raten! Messen! Nsight = Lupe! Zeigt: Kernel 90% Zeit! Kopieren 10%! Dann weisst du: Kernel optimieren! Nicht Kopieren! Raten = Falsch! Messen = Richtig!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Profiling - Tools!

| Tool | Was? |
| --- | --- |
| Nsight Systems! | Ganzes Programm! Wer wartet auf wen? |
| Nsight Compute! | 1 Kernel! Register! Occupancy! |
| nsys profile! | Befehl! Messen! |

### Profiling-Regel!

1. **Erst messen! Dann optimieren!** Nicht raten!

2. **Größter Block zuerst!** 90% Zeit = Dort optimieren!

3. **Vorher / Nachher!** Messen! Ändern! Wieder messen!


```python
# cuda_profiling.py - OMQ CUDA Profiling
import time
def kernel_langsam():
    time.sleep(0.09)  # 90% Zeit!
    return 'Kernel'
def kopieren():
    time.sleep(0.01)  # 10% Zeit!
    return 'Kopieren'
start = time.time()
kernel_langsam()
kopieren()
gesamt = time.time() - start
print(f'Gesamt: {gesamt:.2f}s')
print('Kernel: 90% - DORT optimieren!')
print('Kopieren: 10% - Nicht wichtig!')
print('Profiling: Messen! Nicht raten!')
print('nsys profile ./programm - Dann nsys stats!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL272 R.382 - Nr.136


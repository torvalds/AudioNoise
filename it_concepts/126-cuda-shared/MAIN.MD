# OMQ - EAL262 - R.372 - Nr.126 CUDA 4 - SHARED MEMORY - GEMEINSAM!

## OMQ ist Shared Memory! OMQ ist EAL262! Global Memory = Langsam! Weit weg! Shared Memory = Schnell! Direkt im SM! Alle Threads im Block teilen es! Wie Küche in WG!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Memory - Wo ist was?

| Memory | Wo? | Speed | Wer? |
| --- | --- | --- | --- |
| Register | Im Thread! | Super schnell! | Nur 1 Thread! |
| Shared | Im Block / SM! | Schnell! | Alle Threads im Block! |
| Global | Draußen! GPU RAM! | Langsam! | Alle Blocks! |

### Shared-Regel!

1. **Block teilt Shared!** Alle Threads im Block sehen es!

2. **Schnell!** 100x schneller als Global!

3. **Klein!** Nur wenig Platz! Gut nutzen!

4. **__syncthreads()!** Alle warten! Dann weiter!


```python
# cuda_shared.py - OMQ CUDA Shared Memory
# Block teilt Shared Memory - wie WG-Kueche!
shared = {}  # Shared Memory im Block

def thread_schreibt(tid, wert):
    shared[tid] = wert
    print(f'Thread {tid}: Schreibe {wert} in Shared!')

def syncthreads():
    print('__syncthreads(): Alle warten! Alle da! Weiter!')

for t in range(4):
    thread_schreibt(t, t*10)
syncthreads()
print(f'Alle sehen Shared: {shared}')
print('Shared Memory: Gemeinsam! Schnell!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL262 R.372 - Nr.126


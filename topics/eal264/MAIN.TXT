# OMQ - EAL264 - R.374 - Nr.128 CUDA 6 - SYNC + RACE - WARTEN + NICHT DRAENGELN!

## OMQ ist Sync! OMQ ist Race! OMQ ist EAL264! Sync = Warten! Alle zusammen weiter! Race = Drängeln! 2 Threads schreiben gleiches Fach! Ergebnis = Chaos! Deshalb: Sync + Atomic! Nicht drängeln!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Sync + Race!

| | Was? | Gut? |
| --- | --- | --- |
| __syncthreads() | Alle im Block warten! | JA! Ordnung! |
| Race Condition | 2 schreiben gleich! | NEIN! Chaos! |
| atomicAdd() | Einer nach dem anderen! | JA! Sicher! |

### Sync-Regel!

1. **Schreiben -> Sync -> Lesen!** Erst alle schreiben! Dann alle lesen!

2. **Nie ohne Sync lesen!** Sonst liest du alten Müll!

3. **Atomic = Schlange!** Einer nach dem anderen! Kein Drängeln!


```python
# cuda_sync_race.py - OMQ CUDA Sync + Race
shared = [0] * 4

def sync():
    print('__syncthreads(): Alle warten! Alle da!')

print('FALSCH: Race! Alle schreiben Fach 0 gleichzeitig!')
# Race - Chaos!
print('RICHTIG: Jeder eigenes Fach! Dann Sync! Dann Lesen!')
for t in range(4):
    shared[t] = t * 10
    print(f'Thread {t}: Schreibe Fach {t} = {t*10}')
sync()
print(f'Alle lesen: {shared} - Sauber!')

print('Atomic: Einer nach dem anderen!')
zaehler = 0
for t in range(4):
    zaehler += 1  # atomicAdd - sicher!
    print(f'Thread {t}: atomicAdd -> Zähler = {zaehler}')
print('Sync + Atomic = Kein Chaos!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL264 R.374 - Nr.128


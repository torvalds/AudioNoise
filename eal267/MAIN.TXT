# OMQ - EAL267 - R.377 - Nr.131 CUDA 9 - COALESCED MEMORY - ZUSAMMEN!

## OMQ ist Coalesced! OMQ ist EAL267! Threads lesen nebeneinander = 1 Fahrt! Schnell! Threads lesen wild = 32 Fahrten! Langsam! Wie Einkaufen: Alle zusammen im Wagen = 1 Fahrt! Jeder allein = Chaos!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Coalesced - Zusammen vs Wild!

| | Coalesced! | Wild! |
| --- | --- | --- |
| Threads lesen | Nebeneinander! 0,1,2,3! | Verstreut! 0,100,500! |
| Fahrten | 1 Fahrt! | 32 Fahrten! |
| Speed | Schnell! | Langsam! |

### Coalesced-Regel!

1. **Warp liest zusammen!** 32 Threads = 1 Block Speicher!

2. **threadIdx = Index!** Thread 0 liest Fach 0! Thread 1 liest Fach 1!

3. **Nie springen!** Kein Thread * 100! Kein Zufall!


```python
# cuda_coalesced.py - OMQ CUDA Coalesced Memory
data = list(range(32))
print('RICHTIG: Coalesced! Alle nebeneinander!')
for tid in range(8):
    print(f'Thread {tid} liest Fach {tid} = {data[tid]}')
print('1 Fahrt! Schnell!')
print('FALSCH: Wild! Thread * 10 springt!')
for tid in range(4):
    idx = tid * 10
    print(f'Thread {tid} liest Fach {idx} - Springt! Langsam!')
print('Coalesced: Zusammen! 1 Fahrt!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL267 R.377 - Nr.131


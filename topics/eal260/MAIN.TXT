# OMQ - EAL260 - R.370 - Nr.124 CUDA 2 - WARP + THREAD - 32 FREUNDE!

## OMQ ist Warp! OMQ ist Thread! OMQ ist EAL260! 1 Thread = 1 Arbeiter! 32 Threads = 1 Warp! Warp läuft zusammen! Warp wartet zusammen! Alle 32 gleich!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Warp + Thread - Die Kette!

| Stufe | Wie viele? |
| --- | --- |
| Thread | 1 Arbeiter! |
| Warp | 32 Threads! |
| Block | Viele Warps! |
| Grid | Viele Blocks! |
| SM | Führt Warps aus! |

### Warp-Regel!

1. **Alle 32 gleich!** Gleiche Anweisung!

2. **Wenn einer wartet, warten alle!** Divergenz = langsam!

3. **32 Freunde!** Immer zusammen!


```python
# cuda_warp.py - OMQ CUDA Warp + Thread
# 1 Warp = 32 Threads machen gleiche Rechnung!
warp = list(range(32))
print('Warp startet: 32 Threads!')
results = []
for t in warp:
    r = t * 2  # Alle machen gleiche Rechnung!
    results.append(r)
    print(f'Thread {t}: {t} * 2 = {r}')
print(f'Warp fertig! 32 Ergebnisse: {results[:5]} ...')
print('Warp: 32 Freunde! Alle gleich!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL260 R.370 - Nr.124


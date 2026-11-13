# OMQ - EAL269 - R.379 - Nr.133 CUDA 11 - WARP DIVERGENCE - NICHT AUSEINANDER!

## OMQ ist Divergence! OMQ ist EAL269! Warp = 32 Soldaten! Alle müssen gleich laufen! Wenn if/else: Hälfte links! Hälfte rechts! Warp muss BEIDE Wege laufen! 2x Arbeit! Langsam! Deshalb: Alle gleich!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Divergence - Auseinander vs Zusammen!

| | Gut! Kein Divergence! | Schlecht! Divergence! |
| --- | --- | --- |
| Warp macht | Alle gleich! | Hälfte so, Hälfte so! |
| Wege | 1 Weg! | 2 Wege! |
| Speed | Schnell! | 2x Langsam! |

### Divergence-Regel!

1. **if nach Warp!** Nicht nach Thread! Warp 0 = if! Warp 1 = else!

2. **Kein if mit threadIdx % 2!** Das teilt jeden Warp!

3. **Kein Zufall im Warp!** Alle 32 gleich!


```python
# cuda_divergence.py - OMQ CUDA Warp Divergence
print('SCHLECHT: threadIdx % 2 - Jeder Warp teilt sich!')
for tid in range(8):
    weg = 'links' if tid % 2 == 0 else 'rechts'
    print(f'Thread {tid}: Geht {weg} - Warp muss BEIDE Wege!')
print('GUT: Warp-weise! Warp 0 = links, Warp 1 = rechts!')
for tid in range(8):
    warp = tid // 4  # Warp 0 oder 1
    weg = 'links' if warp == 0 else 'rechts'
    print(f'Thread {tid} Warp {warp}: Geht {weg} - Alle im Warp gleich!')
print('Divergence: Nicht auseinander! Alle gleich!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL269 R.379 - Nr.133


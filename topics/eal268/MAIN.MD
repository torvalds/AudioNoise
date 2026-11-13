# OMQ - EAL268 - R.378 - Nr.132 CUDA 10 - OCCUPANCY - WIE VOLL?

## OMQ ist Occupancy! OMQ ist EAL268! SM = Stadion! Warps = Zuschauer! Stadion leer = Verschwendung! Stadion voll = Schnell! Occupancy = Wie voll ist SM? 100% = Alle Warps aktiv! 25% = Leer! Viel warten!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Occupancy - Wie voll?

| Occupancy | Was? | Speed |
| --- | --- | --- |
| 100% | Stadion voll! Alle Warps da! | Schnell! |
| 50% | Halb voll! | Mittel! |
| 25% | Leer! Viele warten! | Langsam! |

### Occupancy-Regel!

1. **Weniger Register = Mehr Warps!** Viel Register = Wenig Platz!

2. **Weniger Shared = Mehr Blocks!** Viel Shared = Wenig Blocks!

3. **Mehr Blocks = Mehr Warps!** Block 256 Threads = Gut!


```python
# cuda_occupancy.py - OMQ CUDA Occupancy
# SM kann 64 Warps! Wie viele sind aktiv?
MAX_WARPS = 64
def occupancy(aktive_warps):
    pct = aktive_warps * 100 // MAX_WARPS
    print(f'Aktive Warps: {aktive_warps}/{MAX_WARPS} = Occupancy {pct}%')
    return pct
print('SCHLECHT: Viel Register + Viel Shared!')
occupancy(16)
print('GUT: Wenig Register + Wenig Shared!')
occupancy(64)
print('TIPP: Block 256 Threads! Wenig Register! Wenig Shared!')
print('Occupancy: Stadion voll! Schnell!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL268 R.378 - Nr.132


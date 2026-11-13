# OMQ - EAL270 - R.380 - Nr.134 CUDA 12 - BANK CONFLICTS - NICHT GLEICHE BANK!

## OMQ ist Bank! OMQ ist EAL270! Shared Memory = 32 Banken! Jede Bank = 1 Schalter! Wenn 32 Threads gleiche Bank wollen = Schlange! 32x warten! Wenn alle andere Bank = Alle gleichzeitig! Schnell! Deshalb: Nicht gleiche Bank!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Bank Conflicts!

| | Gut! Kein Conflict! | Schlecht! Conflict! |
| --- | --- | --- |
| Threads wollen | Alle andere Bank! | Alle gleiche Bank! |
| Warten | 0! | 32x! |
| Speed | Schnell! | Langsam! |

### Bank-Regel!

1. **Bank = Index % 32!** Fach 0 = Bank 0! Fach 32 = Bank 0! Wieder!

2. **Stride 1 = Gut!** Nebeneinander = Andere Banken!

3. **Stride 32 = Schlecht!** Alle gleiche Bank! Conflict!

4. **Padding hilft!** +1 Fach! Dann andere Bank!


```python
# cuda_bank.py - OMQ CUDA Bank Conflicts
print('SCHLECHT: Stride 32 - Alle gleiche Bank!')
for tid in range(4):
    idx = tid * 32
    bank = idx % 32
    print(f'Thread {tid}: Fach {idx} -> Bank {bank} - CONFLICT!')
print('GUT: Stride 1 - Alle andere Bank!')
for tid in range(8):
    idx = tid
    bank = idx % 32
    print(f'Thread {tid}: Fach {idx} -> Bank {bank} - Kein Conflict!')
print('TIPP: Padding +1! shared[33] statt shared[32]!')
print('Bank: Nicht gleiche Bank! Alle andere!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL270 R.380 - Nr.134


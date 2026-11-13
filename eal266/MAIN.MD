# OMQ - EAL266 - R.376 - Nr.130 CUDA 8 - UNIFIED MEMORY - EIN SPEICHER!

## OMQ ist Unified! OMQ ist EAL266! Früher: CPU-Speicher + GPU-Speicher = 2 Welten! Kopieren hin! Kopieren zurück! Kopfschmerz! Jetzt: Unified Memory = 1 Speicher! CPU + GPU sehen gleiches Fach! Kein Kopieren!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Memory - Früher vs Jetzt!

| | Alt! | Neu! Unified! |
| --- | --- | --- |
| Speicher | 2! CPU + GPU! | 1! Gemeinsam! |
| Kopieren | cudaMemcpy! Immer! | Automatisch! |
| Code | Lang! Fehler! | Kurz! Einfach! |

### Unified-Regel!

1. **cudaMallocManaged() = Ein Speicher!** CPU + GPU beide!

2. **Kein Kopieren!** System macht es!

3. **Achtung: Page Fault!** Erstes Anfassen = Langsam! Dann schnell!


```python
# cuda_unified.py - OMQ CUDA Unified Memory
# 1 Speicher - CPU + GPU sehen gleich!
print('cudaMallocManaged(): 1 Speicher für CPU + GPU!')
data = [1, 2, 3, 4]  # Unified - beide sehen es!
print(f'CPU schreibt: {data}')
print('GPU liest gleiches data - Kein cudaMemcpy!')
result = [x * 10 for x in data]
print(f'GPU rechnet: {result}')
print('CPU liest Ergebnis: Gleicher Speicher!')
print('Unified Memory: 1 Speicher! Kein Kopieren!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL266 R.376 - Nr.130


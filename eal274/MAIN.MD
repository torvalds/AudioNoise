# OMQ - EAL274 - R.384 - Nr.138 CUDA 16 - MULTI-GPU - MEHRERE KARTEN!

## OMQ ist Multi-GPU! OMQ ist EAL274! 1 GPU = 1 Arbeiter! 4 GPUs = 4 Arbeiter! Arbeit teilen! Jeder rechnet sein Stück! Dann zusammen! Aber: Jeder hat eigenen Speicher! Kopieren zwischen GPUs = Langsam! Deshalb: Wenig reden! Viel rechnen!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Multi-GPU!

| | 1 GPU | 4 GPUs |
| --- | --- | --- |
| Arbeiter | 1! | 4! |
| Speed | 1x! | Fast 4x! |
| Problem | Keins! | Kopieren zwischen GPUs! |

### Multi-GPU-Regel!

1. **cudaSetDevice(i) = Wählen!** GPU 0, 1, 2, 3!

2. **Arbeit teilen!** GPU 0 = Hälfte 1! GPU 1 = Hälfte 2!

3. **Wenig Kopieren!** Nur Ergebnis am Ende!

4. **NCCL = Profi!** Für viele GPUs! Macht Kopieren schnell!


```python
# cuda_multi_gpu.py - OMQ CUDA Multi-GPU
data = list(range(100))
print('1 GPU: 100 Zahlen allein!')
print('4 GPUs: Jeder 25 Zahlen!')
gpus = {0: data[0:25], 1: data[25:50], 2: data[50:75], 3: data[75:100]}
for gpu, teil in gpus.items():
    print(f'GPU {gpu}: cudaSetDevice({gpu}) - Rechnet {len(teil)} Zahlen!')
    ergebnis = sum(teil)
    print(f'GPU {gpu}: Ergebnis = {ergebnis}')
print('Alle fertig: Ergebnisse zusammen!')
print('Multi-GPU: Teilen! Rechnen! Zusammen! Wenig Kopieren!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL274 R.384 - Nr.138


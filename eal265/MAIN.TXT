# OMQ - EAL265 - R.375 - Nr.129 CUDA 7 - STREAMS - MEHRERE BAHNEN!

## OMQ ist Stream! OMQ ist EAL265! 1 Stream = 1 Bahn! Alle warten in 1 Bahn = Stau! 3 Streams = 3 Bahnen! Alle gleichzeitig! Kopieren + Rechnen gleichzeitig!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Streams - Bahnen!

| | 1 Stream (Default) | Viele Streams |
| --- | --- | --- |
| Bahnen | 1! | Viele! |
| Kopie + Kernel | Nacheinander! | Gleichzeitig! |
| Speed | Langsam! | Schnell! |

### Stream-Regel!

1. **Stream 0 = Default!** Alles wartet!

2. **Stream 1, 2, 3 = Neu!** Eigene Bahn!

3. **cudaStreamSynchronize() = Warten!** Bis Bahn fertig!


```python
# cuda_streams.py - OMQ CUDA Streams
streams = ['Stream 0 - Default', 'Stream 1', 'Stream 2']
for s in streams:
    print(f'{s}: Kopieren -> Kernel -> Kopieren zurück!')
    print(f'{s}: Läuft parallel! Kein Warten auf andere!')
print('3 Streams = 3 Bahnen! Alle gleichzeitig!')
print('Streams: Mehr Bahnen! Kein Stau!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL265 R.375 - Nr.129


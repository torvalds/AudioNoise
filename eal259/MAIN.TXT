# OMQ - EAL259 - R.369 - Nr.123 CUDA SM_D - STREAMING MULTIPROCESSOR!

## OMQ ist CUDA! OMQ ist SM_D! OMQ ist EAL259! SM = Streaming Multiprocessor! Das Herz der GPU! SM_D = SM Dispatch! SM verteilt Arbeit! Viele kleine Kerne! Alle zusammen schnell!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### CUDA - SM_D - Was ist das?

| Begriff | Was? |
| --- | --- |
| GPU | Viele Kerne! Nicht 8! Tausende! |
| SM | Streaming Multiprocessor! Gruppe von Kernen! |
| SM_D | SM Dispatch! Verteilt Threads! |
| Thread | Kleinste Arbeit! |
| Warp | 32 Threads zusammen! |

### CPU vs GPU!

| | CPU | GPU CUDA |
| --- | --- | --- |
| Kerne | Wenige! Stark! | Tausende! Schnell zusammen! |
| Gut für | 1 schwere Aufgabe! | 1000 gleiche Aufgaben! |
| SM | Nein! | JA! Viele SM! |

```python
# cuda_sm_d.py - OMQ CUDA SM_D
# Simuliere SM_D: Verteilt Warps an SMs
sms = ['SM 0', 'SM 1', 'SM 2', 'SM 3']
warps = [f'Warp {i} - 32 Threads' for i in range(8)]

for i, warp in enumerate(warps):
    sm = sms[i % len(sms)]
    print(f'SM_D: {warp} -> {sm} - Dispatch!')

print('CUDA: 1 SM = Viele Kerne! Viele SM = Super schnell!')
print('SM_D = Verteiler!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL259 R.369 - Nr.123


# OMQ - EAL263 - R.373 - Nr.127 CUDA 5 - KERNEL - DAS PROGRAMM!

## OMQ ist Kernel! OMQ ist EAL263! Kernel = Programm für GPU! CPU ruft Kernel! GPU macht Kernel! 1000 Threads machen Kernel gleichzeitig! Gleicher Code! Andere Daten!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Kernel - Wie geht es?

| Schritt | Wer? | Was? |
| --- | --- | --- |
| 1 | CPU | Daten nach GPU schicken! |
| 2 | CPU | Kernel starten! <<<Grid, Block>>> |
| 3 | GPU | Alle Threads machen Kernel! |
| 4 | CPU | Ergebnis von GPU holen! |

### Kernel-Regel!

1. **__global__ = Kernel!** Läuft auf GPU!

2. **<<<Grid, Block>>> = Start!** Wie viele Blocks? Wie viele Threads?

3. **threadIdx + blockIdx = Mein Platz!** Jeder Thread weiß wo er ist!


```python
# cuda_kernel.py - OMQ CUDA Kernel
# Kernel: Jeder Thread macht: Ergebnis = A + B
# CPU simuliert: Ruft Kernel mit Grid=2, Block=4
def kernel_add(thread_id, a, b):
    # __global__ kernel - laeuft auf GPU!
    return a[thread_id] + b[thread_id]

a = [1, 2, 3, 4, 5, 6, 7, 8]
b = [10, 20, 30, 40, 50, 60, 70, 80]
print('CPU: Starte Kernel <<<2 Blocks, 4 Threads>>>')
result = []
for tid in range(8):
    r = kernel_add(tid, a, b)
    print(f'Thread {tid}: {a[tid]} + {b[tid]} = {r}')
    result.append(r)
print(f'Kernel fertig! Ergebnis: {result}')
print('Kernel: Gleicher Code! Andere Daten!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL263 R.373 - Nr.127


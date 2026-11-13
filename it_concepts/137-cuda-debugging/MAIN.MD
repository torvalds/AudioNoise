# OMQ - EAL273 - R.383 - Nr.137 CUDA 15 - DEBUGGING - WO IST DER FEHLER?

## OMQ ist Debugging! OMQ ist EAL273! CUDA Fehler? Nicht panisch! 3 Fehler = 90% aller Fehler! 1. Falscher Index! 2. Kein Sync! 3. Kein Fehler-Check! cudaGetLastError() = Arzt! Sagt dir: Wo tut es weh?

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Debugging - Top 3 Fehler!

| Fehler | Zeichen | Fix |
| --- | --- | --- |
| Falscher Index! | Absturz! Müll! | if (idx < N) prüfen! |
| Kein Sync! | Falsches Ergebnis! | __syncthreads()! |
| Kein Check! | Fehler unsichtbar! | cudaGetLastError()! |

### Debugging-Regel!

1. **Nach jedem Kernel: Check!** cudaGetLastError()!

2. **compute-sanitizer!** Findet Index-Fehler! `compute-sanitizer ./programm`!

3. **Klein anfangen!** 1 Block! 1 Thread! Dann größer!


```python
# cuda_debugging.py - OMQ CUDA Debugging
def cuda_check(fehler):
    if fehler != 'OK':
        print(f'FEHLER: {fehler} - Hier stoppen!')
        return False
    print('OK: Kein Fehler!')
    return True
print('Nach Kernel 1:')
cuda_check('OK')
print('Nach Kernel 2: Index-Fehler!')
cuda_check('Invalid Argument - Index zu gross!')
print('FIX: if (idx < N) prüfen!')
print('TIPP: compute-sanitizer ./programm = Findet Fehler!')
print('Debugging: Check! Klein! Finden!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL273 R.383 - Nr.137


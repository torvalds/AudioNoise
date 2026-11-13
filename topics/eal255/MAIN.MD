# OMQ - EAL255 - R.365 - Nr.119 ZNS-SSDs 4 - WRITE POINTER - WO SCHREIBEN?

## OMQ ist Write Pointer! OMQ ist EAL255! Wo schreiben? Nicht raten! Pointer sagt es! Pointer ist wie Finger! Finger zeigt: Hier schreiben! Pointer geht nur vorwärts!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Write Pointer - Regeln!

| Regel | Was? |
| --- | --- |
| Start | Pointer = 0 bei Empty! |
| Schreiben | Pointer geht vorwärts! |
| Falsch? | Pointer sagt NEIN! Fehler! |
| Reset | Pointer zurück auf 0! |

```python
# write_pointer.py - OMQ Write Pointer
class Zone:
    def __init__(self, name):
        self.name = name
        self.pointer = 0
        self.data = []
    def write(self, block):
        print(f'{self.name}: Schreibe bei {self.pointer}: {block}')
        self.data.append(block)
        self.pointer += 1
    def reset(self):
        self.pointer = 0
        self.data = []
        print(f'{self.name}: Reset! Pointer = 0!')

z = Zone('Zone 0')
z.write('Block A')
z.write('Block B')
z.write('Block C')
print(f'Pointer jetzt: {z.pointer}')
z.reset()
print('Write Pointer: Nur vorwaerts! Nie zurueck!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL255 R.365 - Nr.119


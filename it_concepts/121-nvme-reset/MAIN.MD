# OMQ - EAL257 - R.367 - Nr.121 NVMe + ZNS 6 - RESET ZONE - NEU ANFANGEN!

## OMQ ist NVMe! OMQ ist Reset Zone! OMQ ist EAL257! NVMe = Schneller Weg! PCIe! Direkt zur CPU! Kein SATA! Kein langsam! Und Reset Zone = Zone leeren! Neu anfangen! Pointer auf 0!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### NVMe - Warum schnell?

| | SATA | NVMe |
| --- | --- | --- |
| Weg | Umweg! | Direkt! PCIe! |
| Speed | Langsam! | Sehr schnell! |
| ZNS | Nein! | JA! ZNS liebt NVMe! |

### Reset Zone - So geht es!

| Schritt | Was? |
| --- | --- |
| 1 | Zone Full? |
| 2 | NVMe Befehl: Zone Reset! |
| 3 | Zone = Empty! Pointer = 0! |
| 4 | Neu schreiben! |

```python
# nvme_reset.py - OMQ NVMe + Reset Zone
class NVMeZone:
    def __init__(self, name):
        self.name = name
        self.pointer = 0
        self.zustand = 'Empty'
    def write(self, daten):
        print(f'NVMe {self.name}: Schreibe bei {self.pointer}: {daten}')
        self.pointer += 1
        self.zustand = 'Open'
    def reset(self):
        print(f'NVMe {self.name}: RESET Befehl!')
        self.pointer = 0
        self.zustand = 'Empty'
        print(f'{self.name}: Neu! Pointer = 0! Zustand = Empty!')

z = NVMeZone('Zone 0')
z.write('Daten A')
z.write('Daten B')
z.reset()
z.write('NEU: Daten C')
print('NVMe + Reset = Schnell + Neu anfangen!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL257 R.367 - Nr.121


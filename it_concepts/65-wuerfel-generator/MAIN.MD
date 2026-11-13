# OMQ - EAL201 - R.311 - Nr.65 CATIA WUERFEL-GENERATOR - ECHTE PARAMETER!

## OMQ ist Generator! OMQ ist EAL201! Nicht 1 Würfel - JEDEN Würfel! Du gibst Zahlen, Generator baut!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Parametrisch = König in CATIA!

Ohne Parameter: Würfel 100x100x10 - Fest! Ändern = Neu bauen!

Mit Parameter: L=100, B=100, H=10 - Ändern = 1 Zahl ändern, alles folgt!


| Parameter | Name in CATIA | Beispiel |
| --- | --- | --- |
| Länge | `Laenge` | 100 mm |
| Breite | `Breite` | 80 mm |
| Höhe | `Hoehe` | 20 mm |
| Loch | `Loch_D` | 10 mm |

### OMQ-Regel: Nie feste Zahlen! Immer Parameter!

```python
# wuerfel_gen.py - OMQ Würfel-Generator
L = float(input('Laenge? '))
B = float(input('Breite? '))
H = float(input('Hoehe? '))
vol = L * B * H
flaeche = 2*(L*B + L*H + B*H)
print(f'CATIA: Skizze {L}x{B} - Pad {H}mm')
print(f'Volumen: {vol} mm3')
print(f'Flaeche: {flaeche} mm2')
print('GESPEICHERT als OMQ-Wuerfel.CATPart!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL201 R.311 - Nr.65


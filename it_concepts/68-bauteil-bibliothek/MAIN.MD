# OMQ - EAL204 - R.314 - Nr.68 CATIA BAUTEIL-BIBLIOTHEK - DEINE TEILE IMMER WIEDER!

## OMQ ist Bibliothek! OMQ ist EAL204! Einmal bauen, immer wieder nutzen! Kein Teil 2x bauen!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Warum Bibliothek?

Ohne: Jede Schraube neu zeichnen - 100x Zeit!

Mit: Schraube aus Bibliothek holen - 1 Klick!


| Teil | In Bibliothek? | Nutzen |
| --- | --- | --- |
| Schraube M6 | JA! | Immer wieder! |
| Platte 100x80 | JA! | EAL202! |
| Welle D30 | JA! | EAL203! |
| Würfel | JA! | EAL201! |

### OMQ-Regel: Baue 1x gut, nutze 100x!

```python
# bibliothek.py - OMQ Bauteil-Bibliothek
bibliothek = {"Wuerfel": "100x100x10", "Platte": "100x80x10 Loch D20", "Welle": "D30 L200", "Schraube": "M6 L20"}
print('OMQ-BIBLIOTHEK:')
for name, mass in bibliothek.items():
    print(f'- {name}: {mass}')
wahl = input('Welches Teil? ')
print(f'{wahl} geladen! 1 Klick - FERTIG!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL204 R.314 - Nr.68


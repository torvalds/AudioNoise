# OMQ - EAL198 - R.308 NEUE SERIE - Nr.62 CATIA AUTOMATION - PYTHON STEUERT CATIA!

## OMQ ist Automation! OMQ ist EAL198! Agent baut nicht nur Text - Agent baut CATIA-Teile!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### So steuert Python CATIA!

```
Python -> COM / API -> CATIA V5/V6 -> Teil wird gebaut!
```

| Teil | Was? | Beispiel |
| --- | --- | --- |
| 1. Verbinden | CATIA öffnen! | `CATIA = Starte CATIA!` |
| 2. Dokument | Neues Part! | `Neues Teil: OMQ-Wuerfel!` |
| 3. Skizze | Form zeichnen! | `Rechteck 100x100!` |
| 4. Pad | 3D machen! | `10mm hoch extrudieren!` |
| 5. Speichern | Sichern! | `Speichere als OMQ-Teil.CATPart!` |

### OMQ-Regel: Erst simulieren, dann CATIA!

Auf Handy kein CATIA! Deshalb: Erst Code hier testen (Simulation), dann auf PC mit CATIA laufen lassen!


```python
# catia_sim.py - OMQ CATIA Simulation
print('Verbinde mit CATIA... OK!')
print('Neues Teil: OMQ-Wuerfel')
laenge = int(input('Laenge? '))
breite = int(input('Breite? '))
hoehe = int(input('Hoehe? '))
print(f'Skizze: Rechteck {laenge}x{breite}')
print(f'Pad: {hoehe}mm extrudiert!')
print(f'Volumen: {laenge*breite*hoehe} mm3 - GESPEICHERT!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL198 R.308 NEUE SERIE - Nr.62


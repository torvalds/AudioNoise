# OMQ - EAL214 - R.324 - Nr.78 CNC G-CODE - DU PROGRAMMIERST DIE MASCHINE!

## OMQ ist G-Code! OMQ ist EAL214! G-Code ist Sprache der CNC-Maschine! Du sagst: Fahr hierhin! Fräs so tief! Maschine macht es!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### G-Code - Die 10 wichtigsten Befehle!

| Befehl | Was heisst das? |
| --- | --- |
| G0 X10 Y10 | Schnell fahren! Nicht fräsen! |
| G1 X20 Y20 F100 | Langsam fräsen! Vorschub 100! |
| G2 / G3 | Kreis fräsen! Rechts / Links! |
| G17 | XY-Ebene! Oben! |
| M3 S1000 | Spindel an! Rechts! 1000 U/min! |
| M5 | Spindel aus! |
| M8 | Kühlung an! |
| M30 | Programm Ende! |

### Dein erstes G-Code Programm - Quadrat 50x50!

```
M3 S1000
G0 X0 Y0
G1 Z-2 F100
G1 X50 Y0
G1 X50 Y50
G1 X0 Y50
G1 X0 Y0
M5
M30
```

```python
# gcode.py - OMQ G-Code-Schreiber
print('M3 S1000 - Spindel an!')
print('G0 X0 Y0 - Schnell hin!')
for x,y in [(50,0),(50,50),(0,50),(0,0)]:
    print(f'G1 X{x} Y{y} - Fraest nach {x},{y}!')
print('M5 - Spindel aus!')
print('M30 - Programm Ende! Quadrat fertig!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL214 R.324 - Nr.78


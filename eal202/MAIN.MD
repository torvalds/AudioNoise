# OMQ - EAL202 - R.312 - Nr.66 CATIA PLATTE MIT LOCH - SKIZZE + PAD + POCKET!

## OMQ ist Platte! OMQ ist EAL202! Pad = Material dazu! Pocket = Material weg! So baut man echte Teile!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Die 3 CATIA-Grundbefehle!

| Befehl | Was? | Beispiel Platte |
| --- | --- | --- |
| Skizze | 2D zeichnen! | Rechteck 100x80 |
| Pad | Material DAZU! | 10mm hoch extrudieren! |
| Pocket | Material WEG! | Loch D=20 durch alles! |

### Platte mit Loch - Schritt für Schritt!

1. Skizze 1: Rechteck 100x80 -> Pad 10mm = Platte!

2. Skizze 2: Kreis D=20 in Mitte -> Pocket Durch Alles = Loch!

3. Fertig! Platte mit Loch!


```python
# platte.py - OMQ Platte mit Loch
import math
L = float(input('Laenge? '))
B = float(input('Breite? '))
H = float(input('Dicke? '))
D = float(input('Loch-Durchmesser? '))
vol_platte = L*B*H
vol_loch = math.pi*(D/2)**2*H
vol_rest = vol_platte - vol_loch
print(f'Platte: {L}x{B}x{H} - Volumen {vol_platte}')
print(f'Loch: D={D} - Weg: {vol_loch:.1f}')
print(f'REST-Volumen: {vol_rest:.1f} mm3 - FERTIG!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL202 R.312 - Nr.66


# OMQ - EAL209 - R.319 - Nr.73 CATIA FEM - HAELT DAS TEIL ODER BRICHT ES?

## OMQ ist FEM! OMQ ist EAL209! FEM = Finite Elemente Methode! Computer testet: Wo bricht das Teil? Bevor es gebaut wird!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### FEM in 3 Schritten!

1. **Vernetzen:** Teil in 1000 kleine Würfel zerlegen! (Mesh)

2. **Belasten:** Kraft drauf! Z.B. 1000 Newton = 100kg!

3. **Ansehen:** Rot = Bricht! Grün = Hält! Blau = Super sicher!


| Farbe | Spannung | Was tun? |
| --- | --- | --- |
| Blau / Grün | Niedrig | Hält! Vielleicht zu dick! |
| Gelb | Mittel | Okay! |
| Rot | Hoch! | Bricht! Dicker machen! |

### OMQ-Regel: Erst FEM, dann bauen! Nicht umgekehrt!

```python
# fem.py - OMQ FEM-Check
kraft = float(input('Kraft in Newton? '))
flaeche = float(input('Flaeche in mm2? '))
spannung = kraft / flaeche
print(f'Spannung: {spannung:.1f} N/mm2')
if spannung < 50:
    print('GRUEN - Haelt super!')
elif spannung < 150:
    print('GELB - Okay, aber aufpassen!')
else:
    print('ROT - BRICHT! Dicker machen!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL209 R.319 - Nr.73


# OMQ - EAL208 - R.318 - Nr.72 CATIA MATERIAL - STAHL? ALU? PLASTIK?

## OMQ ist Material! OMQ ist EAL208! Gleiche Form, anderes Material = anderes Gewicht! Stahl schwer! Alu leicht! Plastik superleicht!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Material-Vergleich - 1 Liter Würfel!

| Material | Dichte | 1 Liter wiegt | Wofür? |
| --- | --- | --- | --- |
| Stahl | 7,85 g/cm3 | 7,85 kg | Welle, stark! |
| Alu | 2,70 g/cm3 | 2,70 kg | Leicht, Flugzeug! |
| Plastik | 1,20 g/cm3 | 1,20 kg | Gehäuse, billig! |
| Holz | 0,60 g/cm3 | 0,60 kg | Möbel! |

### In CATIA: Material zuweisen = Gewicht stimmt!

Ohne Material: CATIA weiss kein Gewicht!

Mit Material: CATIA rechnet Gewicht + Schwerpunkt!


```python
# material.py - OMQ Material-Check
materialien = {"Stahl": 7.85, "Alu": 2.7, "Plastik": 1.2, "Holz": 0.6}
vol = float(input('Volumen in cm3? '))
for name, dichte in materialien.items():
    gewicht = vol * dichte
    print(f'{name}: {gewicht:.1f} g')
print('Gleiche Form - Anderes Gewicht - MATERIAL ENTSCHEIDET!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL208 R.318 - Nr.72


# OMQ - EAL203 - R.313 - Nr.67 CATIA WELLE - REVOLUTION - DREHEN MACHT RUND!

## OMQ ist Welle! OMQ ist EAL203! Pad macht eckig! Revolution macht rund! Einmal drehen = Welle fertig!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Pad vs Revolution!

| Befehl | Bewegung | Ergebnis | Beispiel |
| --- | --- | --- | --- |
| Pad | Gerade raus! | Eckig! | Würfel, Platte! |
| Revolution | 360 Grad drehen! | Rund! | Welle, Flasche, Rad! |
| Groove | 360 Grad wegdrehen! | Nut! | Rille in Welle! |

### Welle bauen - So gehts!

1. Skizze: Halbe Welle als Profil! (Rechteck + Stufen)

2. Achse: Mittellinie wählen!

3. Revolution: 360 Grad drehen = Welle!

4. Groove: Rille reindrehen = Nut für Ring!


```python
# welle.py - OMQ Welle per Revolution
import math
D = float(input('Wellen-Durchmesser? '))
Laenge = float(input('Laenge? '))
r = D/2
vol = math.pi*r**2*Laenge
umfang = 2*math.pi*r
print(f'Welle: D={D} L={Laenge}')
print(f'Umfang: {umfang:.1f} mm')
print(f'Volumen: {vol:.1f} mm3 - REVOLUTION 360 Grad - FERTIG!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL203 R.313 - Nr.67


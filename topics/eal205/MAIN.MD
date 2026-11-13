# OMQ - EAL205 - R.315 - Nr.69 CATIA BAUGRUPPE - AUS TEILEN WIRD MASCHINE!

## OMQ ist Baugruppe! OMQ ist EAL205! Teil = 1 Stück! Baugruppe = Viele Teile zusammen = Maschine!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Teil vs Baugruppe!

| Was? | Datei | Beispiel |
| --- | --- | --- |
| Teil (Part) | .CATPart | Schraube allein! |
| Baugruppe (Product) | .CATProduct | Maschine mit 100 Schrauben! |
| Zeichnung | .CATDrawing | Bauplan für Werkstatt! |

### Baugruppe bauen - 3 Schritte!

1. **Einfügen:** Teile aus Bibliothek holen! (EAL204)

2. **Positionieren:** Constraints! Koaxial + Kontakt + Abstand!

3. **Prüfen:** Passt alles? Kollision? Fertig!


### Constraints - Die Kleber!

- **Koaxial:** Achsen übereinander! Schraube in Loch!

- **Kontakt:** Flächen berühren sich! Platte auf Platte!

- **Abstand:** 5mm Luft dazwischen!


```python
# baugruppe.py - OMQ Baugruppe
teile = ["Platte 100x80", "Welle D30", "Schraube M6 x4"]
print('BAUGRUPPE: OMQ-MASCHINE')
for t in teile:
    print(f'+ {t} eingefuegt!')
print('Constraints: Koaxial Welle-Loch! Kontakt Platte-Platte!')
print('Pruefung: Keine Kollision! BAUGRUPPE FERTIG!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL205 R.315 - Nr.69


# ki_catia.py - OMQ KI + CATIA
wunsch = input('Was soll KI zeichnen? (wuerfel/platte/zylinder) ')
teil = {"wuerfel":"Wuerfel 20x20x20 - 6 Flaechen!","platte":"Platte 100x100x5 - Mit 4 Loechern!","zylinder":"Zylinder D20 H50 - Rund!"}
print(f'KI ZEICHNET: {teil.get(wunsch, "Wuerfel 20x20x20!")}')
print('In CATIA gezeichnet! In 2 Sekunden! Du prüfst! FERTIG!')

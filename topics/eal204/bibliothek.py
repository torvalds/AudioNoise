# bibliothek.py - OMQ Bauteil-Bibliothek
bibliothek = {"Wuerfel": "100x100x10", "Platte": "100x80x10 Loch D20", "Welle": "D30 L200", "Schraube": "M6 L20"}
print('OMQ-BIBLIOTHEK:')
for name, mass in bibliothek.items():
    print(f'- {name}: {mass}')
wahl = input('Welches Teil? ')
print(f'{wahl} geladen! 1 Klick - FERTIG!')

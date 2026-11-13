# naechstes.py - OMQ Neues Kapitel
print('WAS KOMMT ALS NAECHSTES?')
print('1 = CNC - Maschine fraest dein Teil!')
print('2 = 3D-Druck - Dein Teil wird echt!')
print('3 = Roboter - Roboter baut dein Teil!')
print('4 = KI + CATIA - KI zeichnet fuer dich!')
wahl = input('Deine Wahl (1-4)? ')
kapitel = {"1":"CNC","2":"3D-Druck","3":"Roboter","4":"KI + CATIA"}
print(f'DEIN NAECHSTES KAPITEL: {kapitel.get(wahl, "CNC")} - EAL213 GEHT LOS!')

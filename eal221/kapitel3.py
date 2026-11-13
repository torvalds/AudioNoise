# kapitel3.py - OMQ Kapitel 3
print('WAS KOMMT JETZT?')
print('1 = Roboter - Roboter baut dein Teil!')
print('2 = KI + CATIA - KI zeichnet fuer dich!')
wahl = input('Deine Wahl (1-2)? ')
kapitel = {"1":"Roboter","2":"KI + CATIA"}
print(f'DEIN KAPITEL 3: {kapitel.get(wahl, "Roboter")} - EAL222 GEHT LOS!')

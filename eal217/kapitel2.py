# kapitel2.py - OMQ Kapitel 2
print('WAS KOMMT JETZT?')
print('1 = 3D-Druck - Dein Teil aus Plastik!')
print('2 = Roboter - Roboter baut dein Teil!')
print('3 = KI + CATIA - KI zeichnet fuer dich!')
wahl = input('Deine Wahl (1-3)? ')
kapitel = {"1":"3D-Druck","2":"Roboter","3":"KI + CATIA"}
print(f'DEIN KAPITEL 2: {kapitel.get(wahl, "3D-Druck")} - EAL218 GEHT LOS!')

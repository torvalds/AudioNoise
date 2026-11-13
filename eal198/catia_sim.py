# catia_sim.py - OMQ CATIA Simulation
print('Verbinde mit CATIA... OK!')
print('Neues Teil: OMQ-Wuerfel')
laenge = int(input('Laenge? '))
breite = int(input('Breite? '))
hoehe = int(input('Hoehe? '))
print(f'Skizze: Rechteck {laenge}x{breite}')
print(f'Pad: {hoehe}mm extrudiert!')
print(f'Volumen: {laenge*breite*hoehe} mm3 - GESPEICHERT!')

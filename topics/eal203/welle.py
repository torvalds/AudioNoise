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

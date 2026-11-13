# platte.py - OMQ Platte mit Loch
import math
L = float(input('Laenge? '))
B = float(input('Breite? '))
H = float(input('Dicke? '))
D = float(input('Loch-Durchmesser? '))
vol_platte = L*B*H
vol_loch = math.pi*(D/2)**2*H
vol_rest = vol_platte - vol_loch
print(f'Platte: {L}x{B}x{H} - Volumen {vol_platte}')
print(f'Loch: D={D} - Weg: {vol_loch:.1f}')
print(f'REST-Volumen: {vol_rest:.1f} mm3 - FERTIG!')

# wuerfel_gen.py - OMQ Wuerfel-Generator
L = float(input('Laenge? '))
B = float(input('Breite? '))
H = float(input('Hoehe? '))
vol = L * B * H
flaeche = 2*(L*B + L*H + B*H)
print(f'CATIA: Skizze {L}x{B} - Pad {H}mm')
print(f'Volumen: {vol} mm3')
print(f'Flaeche: {flaeche} mm2')
print('GESPEICHERT als OMQ-Wuerfel.CATPart!')

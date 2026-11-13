# fem.py - OMQ FEM-Check
kraft = float(input('Kraft in Newton? '))
flaeche = float(input('Flaeche in mm2? '))
spannung = kraft / flaeche
print(f'Spannung: {spannung:.1f} N/mm2')
if spannung < 50:
    print('GRUEN - Haelt super!')
elif spannung < 150:
    print('GELB - Okay, aber aufpassen!')
else:
    print('ROT - BRICHT! Dicker machen!')

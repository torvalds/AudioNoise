# toleranz.py - OMQ Toleranz-Check
welle = float(input('Wellen-Durchmesser? '))
loch = float(input('Loch-Durchmesser? '))
spiel = loch - welle
print(f'Spiel: {spiel:.2f} mm')
if spiel > 0:
    print('SPIELPASSUNG - Dreht sich! Gut fuer Welle!')
elif spiel == 0:
    print('UEBERGANG - Mal passt, mal nicht!')
else:
    print('PRESSPASSUNG - Sitzt fest! Nur mit Presse!')

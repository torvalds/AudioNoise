# zns_zonen.py - OMQ ZNS Zonen im Detail
zonen = [('Zone 0','Empty','Schreiben!'),('Zone 1','Open','Weiter!'),('Zone 2','Full','Nur lesen!'),('Zone 3','Closed','Reset!')]
for name, zustand, aktion in zonen:
    print(f'{name}: {zustand} -> {aktion}')
print('ZNS-Regel: Nur vorwaerts! Append only!')

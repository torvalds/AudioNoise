# material.py - OMQ Material-Check
materialien = {"Stahl": 7.85, "Alu": 2.7, "Plastik": 1.2, "Holz": 0.6}
vol = float(input('Volumen in cm3? '))
for name, dichte in materialien.items():
    gewicht = vol * dichte
    print(f'{name}: {gewicht:.1f} g')
print('Gleiche Form - Anderes Gewicht - MATERIAL ENTSCHEIDET!')

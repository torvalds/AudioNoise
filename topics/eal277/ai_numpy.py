# ai_numpy.py - OMQ Python AI NumPy
import numpy as np
print('Python Liste: Einzeln rechnen!')
liste = [1,2,3,4]
print(f'Liste +1: {[x+1 for x in liste]} - Loop noetig!')
print('NumPy Array: Alle auf einmal!')
arr = np.array([1,2,3,4])
print(f'Array +1: {arr + 1} - Kein Loop!')
print(f'Array *2: {arr * 2} - Alle auf einmal!')
print(f'Mittelwert: {np.mean(arr)} - 1 Befehl!')
print('NumPy: Zahlen-Maschine! Fuer AI Pflicht!')

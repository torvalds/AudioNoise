# ai_plot.py - OMQ Python AI Matplotlib
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
print('Matplotlib: Bilder fuer Zahlen!')
x = [1,2,3,4,5]
y = [1,4,9,16,25]
plt.plot(x, y, marker='o')
plt.title('OMQ AI - Training wird besser!')
plt.xlabel('Training')
plt.ylabel('Ergebnis')
plt.savefig('training.png')
print('Bild gespeichert: training.png - Schau es an!')
print('Kurve steigt = AI lernt!')

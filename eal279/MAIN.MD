# OMQ - EAL279 - R.389 - Nr.143 PYTHON AI 4 - MATPLOTLIB - BILDER FÜR ZAHLEN!

## OMQ ist Matplotlib! OMQ ist EAL279! Zahlen allein = Langweilig! Bild = Sofort verstehen! 1000 Zahlen = 1 Linie! Steigt? Fällt? Siehst du sofort! Für AI = Fehler sehen! Training sehen! Ohne Bild = Blind!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Matplotlib!

| | Zahlen | Bild |
| --- | --- | --- |
| Verstehen | Langsam! | Sofort! |
| Fehler sehen | Nein! | Ja! |
| AI Training | Zahlen! | Kurve! |

### Matplotlib-Regel!

1. **import matplotlib.pyplot as plt!** Immer so!

2. **plt.plot(x, y)!** Linie zeichnen!

3. **plt.savefig('bild.png')!** In Termux: Kein Fenster! Speichern!


```python
# ai_plot.py - OMQ Python AI Matplotlib
import matplotlib
matplotlib.use('Agg')  # Kein Fenster in Termux!
import matplotlib.pyplot as plt
print('Matplotlib: Bilder für Zahlen!')
x = [1,2,3,4,5]
y = [1,4,9,16,25]
plt.plot(x, y, marker='o')
plt.title('OMQ AI - Training wird besser!')
plt.xlabel('Training')
plt.ylabel('Ergebnis')
plt.savefig('training.png')
print('Bild gespeichert: training.png - Schau es an!')
print('Kurve steigt = AI lernt!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL279 R.389 - Nr.143


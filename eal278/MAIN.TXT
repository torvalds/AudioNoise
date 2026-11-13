# OMQ - EAL278 - R.388 - Nr.142 PYTHON AI 3 - PANDAS - TABELLEN-MEISTER!

## OMQ ist Pandas! OMQ ist EAL278! AI braucht Daten! Daten = Tabelle! Excel = Tabelle! Pandas = Excel in Python! Aber 100x mächtiger! Zeilen filtern! Spalten rechnen! Mittelwert in 1 Befehl! Für AI = Daten putzen!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### Pandas!

| | Excel | Pandas |
| --- | --- | --- |
| Tabelle | Ja! | Ja! DataFrame! |
| 1 Mio Zeilen | Absturz! | Kein Problem! |
| Filtern | Klicken! | 1 Befehl! |

### Pandas-Regel!

1. **import pandas as pd!** Immer so!

2. **pd.DataFrame(daten)!** Dict -> Tabelle!

3. **df['Alter'].mean()!** Mittelwert!

4. **df[df['Alter'] > 30]!** Nur über 30!


```python
# ai_pandas.py - OMQ Python AI Pandas
import pandas as pd
print('Pandas: Tabellen-Meister!')
daten = {'Name': ['Kai','Olaf','OMQ'], 'Alter': [30, 25, 5], 'Stadt': ['Berlin','Berlin','Berlin']}
df = pd.DataFrame(daten)
print(df)
print(f'Mittelwert Alter: {df["Alter"].mean()}')
print('Nur über 10 Jahre:')
print(df[df['Alter'] > 10])
print('Pandas: Daten putzen! Für AI!')
```

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL278 R.388 - Nr.142


# ai_pandas.py - OMQ Python AI Pandas
import pandas as pd
print('Pandas: Tabellen-Meister!')
daten = {'Name': ['Kai','Olaf','OMQ'], 'Alter': [30, 25, 5], 'Stadt': ['Berlin','Berlin','Berlin']}
df = pd.DataFrame(daten)
print(df)
print(f'Mittelwert Alter: {df["Alter"].mean()}')
print('Nur ueber 10 Jahre:')
print(df[df['Alter'] > 10])
print('Pandas: Daten putzen! Fuer AI!')
